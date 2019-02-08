#include "my_malloc.h"

struct _Node {
  size_t size;
  struct _Node *next;
  struct _Node *prev;
  bool FreeFlag;
} Node_t;

typedef struct _Node Node;

Node *head = NULL;
Node *tail = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

__thread Node *head_no = NULL;
__thread Node *tail_no = NULL;
pthread_mutex_t lock_sbrk = PTHREAD_MUTEX_INITIALIZER;

void *distribute(Node *n, size_t size) {
  n->FreeFlag = false;
  if (n->size > size + sizeof(Node)) {
    void *tmp = (void *)n + size + sizeof(Node);
    Node *newblk = (Node *)tmp;
    newblk->size = n->size - size - sizeof(Node);
    newblk->next = n->next;
    newblk->prev = n;
    newblk->FreeFlag = true;

    n->size = size;
    n->next = newblk;
    if (n->next == NULL) {
      tail = newblk;
    } else {
      n->next->prev = newblk;
    }
  }
  pthread_mutex_unlock(&lock);
  return n + 1;
}

void *initialize(size_t size) {
  Node *n;
  if ((n = sbrk(size + sizeof(Node))) == (void *)(-1)) {
    pthread_mutex_unlock(&lock);
    return NULL;
  }
  n->size = size;
  n->next = NULL;
  n->prev = NULL;
  n->FreeFlag = false;
  head = n;
  tail = n;
  pthread_mutex_unlock(&lock);
  return n + 1;
}

void *create(size_t size) {
  Node *n;
  if ((n = sbrk(size + sizeof(Node))) == (void *)(-1)) {
    pthread_mutex_unlock(&lock);
    return NULL;
  }
  n->size = size;
  n->next = NULL;
  n->prev = tail;
  n->FreeFlag = false;
  tail->next = n;
  tail = n;
  pthread_mutex_unlock(&lock);
  return n + 1;
}

void *ts_malloc_lock(size_t size) {
  if (!head) {
    return initialize(size);
  }

  pthread_mutex_lock(&lock);
  Node *ptr = head;
  Node *exist = NULL;
  size_t min_size = INT_MAX;

  while (ptr) {
    if (ptr->FreeFlag && ptr->size >= size) {
      if (ptr->size == size) {
        exist = ptr;
        break;
      }
      if (ptr->size < min_size) {
        exist = ptr;
        min_size = ptr->size;
      }
    }
    ptr = ptr->next;
  }

  if (exist) {
    return distribute(exist, size);
  } else {
    return create(size);
  }
}

void coalesce(Node *n) {
  if (n->prev) {
    if (n->prev->FreeFlag) {
      n->prev->next = n->next;
      if (n->next) {
        n->next->prev = n->prev;
      }
      n = n->prev;
    }
  }
  if (n->next) {
    if (n->next->FreeFlag) {
      n->next = n->next->next;
      if (n->next) {
        n->next->prev = n;
      }
    }
  }
}

void ts_free_lock(void *ptr) {
  pthread_mutex_lock(&lock);
  Node *n = ptr - sizeof(Node);
  n->FreeFlag = true;
  coalesce(n);
  pthread_mutex_unlock(&lock);
}

void *distribute_no(Node *n, size_t size) {
  n->FreeFlag = false;
  if (n->size > size + sizeof(Node)) {
    void *tmp = (void *)n + size + sizeof(Node);
    Node *newblk = (Node *)tmp;
    newblk->size = n->size - size - sizeof(Node);
    newblk->next = n->next;
    newblk->prev = n;
    newblk->FreeFlag = true;

    n->size = size;
    n->next = newblk;
    if (n->next == NULL) {
      tail_no = newblk;
    } else {
      n->next->prev = newblk;
    }
  }
  return n + 1;
}

void *initialize_no(size_t size) {
  pthread_mutex_lock(&lock_sbrk);
  Node *n;
  if ((n = sbrk(size + sizeof(Node))) == (void *)(-1)) {
    pthread_mutex_unlock(&lock_sbrk);
    return NULL;
  }
  pthread_mutex_unlock(&lock_sbrk);
  n->size = size;
  n->next = NULL;
  n->prev = NULL;
  n->FreeFlag = false;
  head_no = n;
  tail_no = n;
  return n + 1;
}

void *create_no(size_t size) {
  pthread_mutex_lock(&lock_sbrk);
  Node *n;
  if ((n = sbrk(size + sizeof(Node))) == (void *)(-1)) {
    pthread_mutex_unlock(&lock_sbrk);
    return NULL;
  }
  pthread_mutex_unlock(&lock_sbrk);
  n->size = size;
  n->next = NULL;
  n->prev = tail_no;
  n->FreeFlag = false;
  tail_no->next = n;
  tail_no = n;
  return n + 1;
}

void *ts_malloc_nolock(size_t size) {
  if (!head_no) {
    return initialize_no(size);
  }

  Node *ptr = head_no;
  Node *exist = NULL;
  size_t min_size = INT_MAX;

  while (ptr) {
    if (ptr->FreeFlag && ptr->size >= size) {
      if (ptr->size == size) {
        exist = ptr;
        break;
      }
      if (ptr->size < min_size) {
        exist = ptr;
        min_size = ptr->size;
      }
    }
    ptr = ptr->next;
  }

  if (exist) {
    return distribute_no(exist, size);
  } else {
    return create_no(size);
  }
}

void coalesce_no(Node *n) {
  if (n->prev) {
    if (n->prev->FreeFlag) {
      n->prev->next = n->next;
      if (n->next) {
        n->next->prev = n->prev;
      }
      n = n->prev;
    }
  }
  if (n->next) {
    if (n->next->FreeFlag) {
      n->next = n->next->next;
      if (n->next) {
        n->next->prev = n;
      }
    }
  }
}

void ts_free_nolock(void *ptr) {
  Node *n = ptr - sizeof(Node);
  n->FreeFlag = true;
  coalesce_no(n);
}
