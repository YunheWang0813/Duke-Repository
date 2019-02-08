"""
Math 590
Project 1 - Sorting Algorithms
Fall 2018

Partner 1: Yunhe Wang (yw314)
Partner 2: Haili Wu (hw210)
Date: 2018/10/28
"""

# Import time, random, plotting, stats, and numpy.
import time
import random
import matplotlib.pyplot as plt
import scipy.stats as stats
import numpy

"""
SelectionSort
"""
# SelectionSort(A):
# The function iteratively search the unsorted components of the array for the 
# minimum element, and switch it with the first element of that unsorted
# components and make it as "sorted components". Finally, as there is no elements
# left in the unsorted components, the sorting is end and the original array
# is sorted. 
# Input: A(a potentially unsorted array)
# Output: A(a sorted array)

def SelectionSort(A):
    # NeedSwap is a variable to check if there is a necessity of swapping elements
    NeedSwap = 0
    # last element of array cannot compare with the elements after that,
    # so range(len(A)-1)
    for i in range(len(A)-1):
        # record the first index of "unsorted array"
        MinIdx = i
        for j in range(i+1, len(A)):
            if A[j] < A[MinIdx]:
                # the index of minimum element is identified
                MinIdx = j
                NeedSwap+=1
        # NeedSwap = 0 indicates that no need of swapping
        if NeedSwap:
            A[i], A[MinIdx] = A[MinIdx], A[i]
        NeedSwap = 0
    return A

"""
InsertionSort
"""
# InsertionSort(A):
# The function stores the index of the array, starting at the front (k = 0), and
# iteratively insert the element at k + 1 into the previous components. The 
# element go back the array one by one until it meets the element smaller than 
# it, or reach the beginning of the array which make the previous components to 
# be "sorted components".
# Input: A(a potentially unsorted array)
# Output: A(a sorted array)

def InsertionSort(A):
    for i in range(1, len(A)):
        # starting index of judgement of insertion
        j = i - 1
        while j >= 0:
            # use j+1 as index to make it able to continuously swap the elements
            if A[j] > A[j+1]:
                A[j], A[j+1] = A[j+1], A[j]
            else:
                # if swap is not necessary, just go to next step
                break
            j-=1    
    return A

"""
BubbleSort:
"""
# BubbleSort(A):
# The function iterate through the array and compare every two adjacent elements,
# and if they are out of order, swap them. Repeat swapping and when no more swap 
# is needed, the array is sorted.
# Input: A(a potentially unsorted array)
# Output: A(a sorted array)

def BubbleSort(A):
    # flag is a variable to judge if the program can get out of the loop
    flag = 0
    swapped = 0
    while not flag:
        # start check elements from A[0] to A[len(A)-1] 
        for i in range(len(A)-1):
            #compare consective 2 elements and swap them if left one < right one
            if A[i+1]<A[i]:
                A[i], A[i+1] = A[i+1], A[i]
                swapped += 1
        # when no element is swapped in one iteration, get out of the loop
        if not swapped:
            flag = 1
        # reset the swapped count to avoid infinite loop risk
        swapped = 0
    return A

"""
MergeSort
"""
# Merge(left, right):
# The function performs the merge operation. It takes 2 arrays and compare
# each element starting from index 0. If the left element is smaller than the 
# right element, append it to the "result array" and delete it (pop(0) operation
# is useful). The right one do the same operation. When elements of either of
# the array are all deleted, append the remaining elements to "result array" 
# to make the sorted array of two original arrays.
# Input: left(a potentially unsorted array, "left" side), 
#        right(a potentially unsorted array, "right" side)
# Output: ans(a sorted array)

def Merge(left, right):
    # ans is the array indicates the result
    ans = []
    while len(left) and len(right):
        # we just need to compare the first element of each array because 
        # pop(0) will delete the element of the array that is already appended
        # to ans
        if left[0] < right[0]:
            ans.append(left.pop(0))
        else:
            ans.append(right.pop(0))
    # make sure that the ramaining elements of the array (left or right)
    # are appended to ans
    while len(left):
        ans.append(left.pop(0))
    while len(right):
        ans.append(right.pop(0))    
    return ans

# MergeSort(A):
# The function performs the merge sort operation based on the call of 
# Merge(left, right) function. It recursively call the MergeSort function itself
# to devide the array and merge the parts of array to return the sorted array.
# Input: A(an array)
# Output: A(an array)

def MergeSort(A):
    # base case: the length of an array is bigger than 1
    if len(A) > 1:
        # divided the array to the half: left part and right part
        mid = len(A)//2
        left = A[:mid]
        right = A[mid:]
        # recursive call
        left = MergeSort(left)
        right = MergeSort(right)
        # recursively merge left part and right part
        # Attention: Need copy operation to make the original array A get changed
        A[0:len(A)] = Merge(left, right)[0:len(A)]
    return A

"""
QuickSort
"""
# QuickSort(A,i,j)
# The function performs the quick sort operation based on the call of helper(A,i,j)
# function.It chooses the element on the right end as the pivot then divides current
# array by the pivot.Then call the function recursively until to trigger the base case
# of the helper function.
# Input: A(a potentially unsorted array), 
#        i(start index of current array(inclusive)), 
#        j(end index of current array(inclusive)) 
# Output: A(a sorted array)
# Sort a list A with the call QuickSort(A, 0, len(A)).

def helper(A,i,j):
    # base case: if the length of current array is 1,not need to sort 
    if j - i < 1:
        return A
    # choose the right end element of current array as the pivot
    pivot = A[j] 
    # use two pointers ,use right to traverse all element in current array
    left = i 
    # use left to track boundary of sorted elements
    right = j - 1 
    while left < right:
        if A[right] >= pivot:
            right-=1
        else:
            # put all elements smaller than pivot to the  left side of current array
            A[left] ,A[right] = A[right], A[left] 
            left+=1
    if A[left] < pivot:
        left+=1
    A[j],A[left] = A[left], A[j]
    # divide current array by the pivot
    helper(A,i,left -1) 
    # conquer two children array by recurively calling helper function
    helper(A,left + 1,j) 
    return A

def QuickSort(A,i,j):
    return helper(A,i,j - 1)

"""
isSorted

This function will take in an original unsorted list and a sorted version of
that same list, and return whether the list has been properly sorted.

Note that this function does not change the unsorted list.

INPUTS
unA: the original unsorted list
sA:  the supposedly sorted list

OUTPUTS
returns true or false
"""
def isSorted(unA, sA):
    # Copy the unsorted list.
    temp = unA.copy()
    
    # Use python's sort.
    temp.sort()

    # Check equality.
    return temp == sA

"""
testingSuite

This function will run a number of tests using the input algorithm, check if
the sorting was successful, and print which tests failed (if any).

This is not an exhaustive list of tests by any means, but covers the edge
cases for your sorting algorithms.

INPUTS
alg: a string indicating which alg to test, the options are:
    'SelectionSort'
    'InsertionSort'
    'BubbleSort'
    'MergeSort'
    'QuickSort'

OUTPUTS
Printed statements indicating which tests passed/failed.
"""
def testingSuite(alg):
    # First, we seed the random number generator to ensure reproducibility.
    random.seed(1)

    # List of possible algs.
    algs = ['SelectionSort', 'InsertionSort', \
            'BubbleSort', 'MergeSort', 'QuickSort']

    # Make sure the input is a proper alg to consider.
    if not alg in algs:
        raise Exception('Not an allowed algorithm. Value was: {}'.format(alg))
    
    # Create a list to store all the tests.
    tests = []

    # Create a list to store the test names.
    message = []

    # Test 1: singleton array
    tests.append([1])
    message.append('singleton array')

    # Test 2: repeated elements
    tests.append([1,2,3,4,5,5,4,3,2,1])
    message.append('repeated elements')

    # Test 3: all repeated elements
    tests.append([2,2,2,2,2,2,2,2,2,2])
    message.append('all repeated elements')

    # Test 4: descending order
    tests.append([10,9,8,7,6,5,4,3,2,1])
    message.append('descending order')

    # Test 5: sorted input
    tests.append([1,2,3,4,5,6,7,8,9,10])
    message.append('sorted input')

    # Test 6: negative inputs
    tests.append([-1,-2,-3,-4,-5,-5,-4,-3,-2,-1])
    message.append('negative inputs')

    # Test 7: mixed positive/negative
    tests.append([1,2,3,4,5,-1,-2,-3,-4,-5,0])
    message.append('mixed positive/negative')

    # Test 8: array of size 2^k - 1
    temp = list(range(0,2**6-1))
    random.shuffle(temp)
    tests.append(temp)
    message.append('array of size 2^k - 1')

    # Test 9: random real numbers
    tests.append([random.random() for x in range(0,2**6-1)])
    message.append('random real numbers')

    # Store total number of passed tests.
    passed = 0

    # Loop over the tests.
    for tInd in range(0,len(tests)):
        # Copy the test for sorting.
        temp = tests[tInd].copy()

        # Try to sort, but allow for errors.
        try:
            # Do the sort.
            eval('%s(tests[tInd])' % alg) if alg != 'QuickSort' \
            else eval('%s(tests[tInd],0,len(tests[tInd]))' % alg)
            
            # Check if the test succeeded.
            if isSorted(temp, tests[tInd]):
                print('Test %d Success: %s' % (tInd+1, message[tInd]))
                passed += 1
            else:
                print('Test %d FAILED: %s' % (tInd+1, message[tInd]))

        # Catch any errors.
        except Exception as e:
            print('')
            print('DANGER!')
            print('Test %d threw an error: %s' % (tInd+1, message[tInd]))
            print('Error: ')
            print(e)
            print('')

    # Done testing, print and return.
    print('')
    print('%d/9 Tests Passed' % passed)
    return

"""
measureTime

This function will generate lists of varying lengths and sort them using your
implemented fuctions. It will time these sorting operations, and store the
average time across 30 trials of a particular size n. It will then create plots
of runtime vs n. It will also output the slope of the log-log plots generated
for several of the sorting algorithms.

INPUTS
sortedFlag: set to True to test with only pre-sorted inputs
    (default = False)
numTrials: the number of trials to average timing data across
    (default = 30)

OUTPUTS
A number of genereated runtime vs n plot, a log-log plot for several
algorithms, and printed statistics about the slope of the log-log plots.
"""
def measureTime(sortedFlag = False, numTrials = 30):
    # Print whether we are using sorted inputs.
    if sortedFlag:
        print('Timing algorithms using only sorted data.')
    else:
        print('Timing algorithms using random data.')
    print('')
    print('Averaging over %d Trials' % numTrials)
    print('')
    
    # First, we seed the random number generator to ensure consistency.
    random.seed(1)

    # We now define the range of n values to consider.
    if sortedFlag:
        # Need to look at larger n to get a good sense of runtime.
        # Look at n from 20 to 980.
        # Note that 1000 causes issues with recursion depth...
        N = list(range(1,50))
        N = [20*x for x in N]
    else:
        # Look at n from 10 to 500.
        N = list(range(1,51))
        N = [10*x for x in N]

    # Store the different algs to consider.
    algs = ['SelectionSort', 'InsertionSort', \
            'BubbleSort', 'MergeSort', \
            'QuickSort', 'list.sort']

    # Preallocate space to store the runtimes.
    tSelectionSort = N.copy()
    tInsertionSort = N.copy()
    tBubbleSort = N.copy()
    tMergeSort = N.copy()
    tQuickSort = N.copy()
    tPython = N.copy()

    # Create some flags for whether each sorting alg works.
    correctFlag = [True, True, True, True, True, True]

    # Loop over the different sizes.
    for nInd in range(0,len(N)):
        # Get the current value of n to consider.
        n = N[nInd]
        
        # Reset the running sum of the runtimes.
        timing = [0,0,0,0,0,0]
        
        # Loop over the 30 tests.
        for test in range(1,numTrials+1):
            # Create the random list of size n to sort.
            A = list(range(0,n))
            A = [random.random() for x in A]

            if sortedFlag:
                # Pre-sort the list.
                A.sort()

            # Loop over the algs.
            for aI in range(0,len(algs)):
                # Grab the name of the alg.
                alg = algs[aI]

                # Copy the original list for sorting.
                B = A.copy()
                
                # Time the sort.
                t = time.time()
                eval('%s(B)' % alg) if aI!=4 else eval('%s(B,0,len(B))' % alg)
                t = time.time() - t

                # Ensure that your function sorted the list.
                if not isSorted(A,B):
                    correctFlag[aI] = False

                # Add the time to our running sum.
                timing[aI] += t

        # Now that we have completed the numTrials tests, average the times.
        timing = [x/numTrials for x in timing]

        # Store the times for this value of n.
        tSelectionSort[nInd] = timing[0]
        tInsertionSort[nInd] = timing[1]
        tBubbleSort[nInd] = timing[2]
        tMergeSort[nInd] = timing[3]
        tQuickSort[nInd] = timing[4]
        tPython[nInd] = timing[5]

    # If there was an error in one of the plotting algs, report it.
    for aI in range(0,len(algs)-1):
        if not correctFlag[aI]:
            print('%s not implemented properly!!!' % algs[aI])
            print('')

    # Now plot the timing data.
    for aI in range(0,len(algs)):
        # Get the alg.
        alg = algs[aI] if aI != 5 else 'Python'

        # Plot.
        plt.figure()
        eval('plt.plot(N,t%s)' % alg)
        plt.title('%s runtime versus n' % alg)
        plt.xlabel('Input Size n')
        plt.ylabel('Runtime (s)')
        if sortedFlag:
            plt.savefig('%s_sorted.png' % alg, bbox_inches='tight')
        else:
            plt.savefig('%s.png' % alg, bbox_inches='tight')

    # Plot them all together.
    plt.figure()
    fig, ax = plt.subplots()
    ax.plot(N,tSelectionSort, label='Selection')
    ax.plot(N,tInsertionSort, label='Insertion')
    ax.plot(N,tBubbleSort, label='Bubble')
    ax.plot(N,tMergeSort, label='Merge')
    ax.plot(N,tQuickSort, label='Quick')
    ax.plot(N,tPython, label='Python')
    legend = ax.legend(loc='upper left')
    plt.title('All sorting runtimes versus n')
    plt.xlabel('Input Size n')
    plt.ylabel('Runtime (s)')
    if sortedFlag:
        plt.savefig('sorting_sorted.png', bbox_inches='tight')
    else:
        plt.savefig('sorting.png', bbox_inches='tight')

    # Now look at the log of the sort times.
    logN = [(numpy.log(x) if x>0 else -6) for x in N]
    logSS = [(numpy.log(x) if x>0 else -6) for x in tSelectionSort]
    logIS = [(numpy.log(x) if x>0 else -6) for x in tInsertionSort]
    logBS = [(numpy.log(x) if x>0 else -6) for x in tBubbleSort]
    logMS = [(numpy.log(x) if x>0 else -6) for x in tMergeSort]
    logQS = [(numpy.log(x) if x>0 else -6) for x in tQuickSort]

    # Linear regression.
    mSS, _, _, _, _ = stats.linregress(logN,logSS)
    mIS, _, _, _, _ = stats.linregress(logN,logIS)
    mBS, _, _, _, _ = stats.linregress(logN,logBS)

    # Plot log-log figure.
    plt.figure()
    fig, ax = plt.subplots()
    ax.plot(logN,logSS, label='Selection')
    ax.plot(logN,logIS, label='Insertion')
    ax.plot(logN,logBS, label='Bubble')
    legend = ax.legend(loc='upper left')
    plt.title('Log-Log plot of runtimes versus n')
    plt.xlabel('log(n)')
    plt.ylabel('log(runtime)')
    if sortedFlag:
        plt.savefig('log_sorted.png', bbox_inches='tight')
    else:
        plt.savefig('log.png', bbox_inches='tight')

    # Print the regression info.
    print('Selection Sort log-log Slope (all n): %f' % mSS)
    print('Insertion Sort log-log Slope (all n): %f' % mIS)
    print('Bubble Sort log-log Slope (all n): %f' % mBS)
    print('')

    # Now strip off all n<200...
    logN = logN[19:]
    logSS = logSS[19:]
    logIS = logIS[19:]
    logBS = logBS[19:]
    logMS = logMS[19:]
    logQS = logQS[19:]

    # Linear regression.
    mSS, _, _, _, _ = stats.linregress(logN,logSS)
    mIS, _, _, _, _ = stats.linregress(logN,logIS)
    mBS, _, _, _, _ = stats.linregress(logN,logBS)
    mMS, _, _, _, _ = stats.linregress(logN,logMS)
    mQS, _, _, _, _ = stats.linregress(logN,logQS)

    # Print the regression info.
    print('Selection Sort log-log Slope (n>%d): %f' \
          % (400 if sortedFlag else 200, mSS))
    print('Insertion Sort log-log Slope (n>%d): %f' \
          % (400 if sortedFlag else 200, mIS))
    print('Bubble Sort log-log Slope (n>%d): %f' \
          % (400 if sortedFlag else 200, mBS))
    print('Merge Sort log-log Slope (n>%d): %f' \
          % (400 if sortedFlag else 200, mMS))
    print('Quick Sort log-log Slope (n>%d): %f' \
          % (400 if sortedFlag else 200, mQS))

    # Close all figures.
    plt.close('all')
