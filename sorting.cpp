/*
==========================================================================
              ALL SORTING TECHNIQUES - in one file (C++)
==========================================================================

QUICK CHEAT-SHEET
--------------------------------------------------------------------------
Algo        | Time (Best/Avg/Worst)       | Space   | Stable? | One-line trick
--------------------------------------------------------------------------
Bubble      | O(n) / O(n^2) / O(n^2)      | O(1)    | Yes     | Big elements "bubble up" to the end
Selection   | O(n^2) in all cases         | O(1)    | No      | Pick the min, put it in front
Insertion   | O(n) / O(n^2) / O(n^2)      | O(1)    | Yes     | Sorting playing cards in your hand
Merge       | O(n log n) in all cases     | O(n)    | Yes     | Split, sort, merge
Quick       | O(n log n)/O(n log n)/O(n^2)| O(log n)| No      | Place the pivot at its final position
Heap        | O(n log n) in all cases     | O(1)    | No      | Build max-heap, move root to the end
Counting    | O(n + k)                    | O(k)    | Yes     | Count frequencies, write back
Radix       | O(d * (n + k))              | O(n + k)| Yes     | Digit by digit (from last digit), using counting sort
Bucket      | O(n + k) average            | O(n + k)| Yes     | Distribute into range buckets, sort each bucket
Shell       | ~O(n log^2 n) (gap-dependent)| O(1)   | No      | Insertion sort with a gap

INTERVIEW / CP TIPS:
 - Lower bound for comparison-based sorting = O(n log n). Counting/Radix/Bucket
   beat it because they don't compare elements.
 - Stable = equal elements keep their original relative order (useful for
   multi-key sorting).
 - In CP, std::sort is used 99% of the time. But know the concepts.
 - Nearly sorted array -> Insertion sort is best.
 - Linked list sorting -> Merge sort is best.
 - Need guaranteed O(n log n) with O(1) extra memory -> Heap sort.
 - Small integer range -> Counting sort.
==========================================================================
*/

#include <bits/stdc++.h>
using namespace std;

/* -----------------------------------------------------------------------
   1) BUBBLE SORT
   TRICK: "Big ones bubble up". After each pass, the largest remaining
          element reaches its correct place at the end.
   IDEA : Compare adjacent elements, swap if they are in the wrong order.
   TIP  : Use a 'swapped' flag. If a full pass makes no swap, the array is
          already sorted, so break early (best case O(n)).
   ----------------------------------------------------------------------- */
void bubbleSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++) {            // n-1 passes
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {    // last i elements are already in place
            if (a[j] > a[j + 1]) {
                swap(a[j], a[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;                     // optimization
    }
}

/* -----------------------------------------------------------------------
   2) SELECTION SORT
   TRICK: "Pick the minimum, put it in front". Each time, find the minimum
          of the unsorted part and swap it into index i.
   NOTE : At most n-1 swaps, so it's useful when swapping is expensive.
          But comparisons are always O(n^2), and it is not stable.
   ----------------------------------------------------------------------- */
void selectionSort(vector<int>& a) {
    int n = a.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;                          // assume a[i] is the minimum
        for (int j = i + 1; j < n; j++) {
            if (a[j] < a[minIdx]) minIdx = j;    // found a smaller one, update index
        }
        swap(a[i], a[minIdx]);                   // bring the minimum to the front
    }
}

/* -----------------------------------------------------------------------
   3) INSERTION SORT
   TRICK: "Playing cards in hand". Pick the next card and slide it into the
          correct spot among the already-sorted cards on its left (shifting
          bigger ones one step to the right).
   TIP  : Very fast on nearly sorted data. Real libraries also use it for
          small arrays (n < ~16).
   ----------------------------------------------------------------------- */
void insertionSort(vector<int>& a) {
    int n = a.size();
    for (int i = 1; i < n; i++) {
        int key = a[i];                          // the "new card"
        int j = i - 1;
        while (j >= 0 && a[j] > key) {           // shift bigger elements to the right
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;                          // drop the key into its correct place
    }
}

/* -----------------------------------------------------------------------
   4) MERGE SORT  (Divide & Conquer)
   TRICK: "Split, sort, merge". Break the array in the middle, recursively
          sort both halves, then merge the two sorted halves.
   KEY  : The real work happens in merge() (two-pointer technique).
   TIP  : Always O(n log n) and stable, but needs O(n) extra space.
   ----------------------------------------------------------------------- */
void mergeArrays(vector<int>& a, int l, int mid, int r) {
    vector<int> left(a.begin() + l, a.begin() + mid + 1);       // copy of left half
    vector<int> right(a.begin() + mid + 1, a.begin() + r + 1);  // copy of right half

    int i = 0, j = 0, k = l;
    while (i < (int)left.size() && j < (int)right.size()) {
        // <= keeps it stable (on a tie, the left element goes first)
        if (left[i] <= right[j]) a[k++] = left[i++];
        else                     a[k++] = right[j++];
    }
    while (i < (int)left.size())  a[k++] = left[i++];           // leftover from left
    while (j < (int)right.size()) a[k++] = right[j++];          // leftover from right
}

void mergeSort(vector<int>& a, int l, int r) {
    if (l >= r) return;                          // base case: 1 element is already sorted
    int mid = l + (r - l) / 2;
    mergeSort(a, l, mid);                        // sort left half
    mergeSort(a, mid + 1, r);                    // sort right half
    mergeArrays(a, l, mid, r);                   // merge both halves
}

/* -----------------------------------------------------------------------
   5) QUICK SORT  (Divide & Conquer)
   TRICK: "Seat the pivot at its final position". After partitioning, smaller
          elements are on the left, bigger on the right, pivot is exactly
          where it belongs.
   TIP  : Average O(n log n), but a sorted array + bad pivot choice gives
          O(n^2). That's why we use a random pivot or median-of-three.
   NOTE : This uses Lomuto partition (last element as pivot) + random pivot.
   ----------------------------------------------------------------------- */
int partitionLomuto(vector<int>& a, int low, int high) {
    // move a random pivot to the last position (to avoid the worst case)
    int randIdx = low + rand() % (high - low + 1);
    swap(a[randIdx], a[high]);

    int pivot = a[high];
    int i = low - 1;                             // i = boundary of the "smaller" zone
    for (int j = low; j < high; j++) {
        if (a[j] < pivot) {
            i++;
            swap(a[i], a[j]);                    // send smaller element into the left zone
        }
    }
    swap(a[i + 1], a[high]);                     // place pivot at its correct position
    return i + 1;                                // final index of the pivot
}

void quickSort(vector<int>& a, int low, int high) {
    if (low >= high) return;
    int p = partitionLomuto(a, low, high);
    quickSort(a, low, p - 1);                    // part left of the pivot
    quickSort(a, p + 1, high);                   // part right of the pivot
}

/* -----------------------------------------------------------------------
   6) HEAP SORT
   TRICK: "Build a max-heap, send the root (largest) to the end, shrink the
          heap, repeat". A heap is a complete binary tree where every
          parent >= its children.
   INDEX: children of node i -> 2i+1 and 2i+2 (0-indexed).
   TIP  : In-place and guaranteed O(n log n), but not cache-friendly and
          not stable.
   ----------------------------------------------------------------------- */
void heapify(vector<int>& a, int n, int i) {
    int largest = i;
    int left = 2 * i + 1, right = 2 * i + 2;
    if (left < n && a[left] > a[largest])   largest = left;
    if (right < n && a[right] > a[largest]) largest = right;
    if (largest != i) {                          // parent is smaller, push it down
        swap(a[i], a[largest]);
        heapify(a, n, largest);
    }
}

void heapSort(vector<int>& a) {
    int n = a.size();
    // Step 1: build max-heap (start from the last non-leaf node and go up) -> O(n)
    for (int i = n / 2 - 1; i >= 0; i--) heapify(a, n, i);
    // Step 2: move root to the end, shrink heap, heapify -> O(n log n)
    for (int i = n - 1; i > 0; i--) {
        swap(a[0], a[i]);                        // largest element goes to its final place
        heapify(a, i, 0);                        // remaining heap is [0..i-1]
    }
}

/* -----------------------------------------------------------------------
   7) COUNTING SORT  (non-comparison)
   TRICK: "Count and write back". Store how many times each value appears in
          a count array, then build the output from those counts.
   WHEN : When the value range (k) is small (e.g., 0..1e6). Negative numbers
          are handled with an offset of the min value (done below).
   TIP  : Prefix sum + traversing from the back makes it STABLE (needed for
          Radix sort).
   ----------------------------------------------------------------------- */
void countingSort(vector<int>& a) {
    if (a.empty()) return;
    int mn = *min_element(a.begin(), a.end());
    int mx = *max_element(a.begin(), a.end());
    int range = mx - mn + 1;

    vector<int> count(range, 0);
    for (int x : a) count[x - mn]++;             // count frequencies (offset = mn)

    for (int i = 1; i < range; i++)              // prefix sum: count[i] = number of elements <= i
        count[i] += count[i - 1];

    vector<int> out(a.size());
    for (int i = (int)a.size() - 1; i >= 0; i--) { // go from the back -> stable
        out[--count[a[i] - mn]] = a[i];
    }
    a = out;
}

/* -----------------------------------------------------------------------
   8) RADIX SORT  (LSD - Least Significant Digit first)
   TRICK: "Digit by digit, starting from the last digit". Sort by the units
          digit, then tens, then hundreds... Each step uses a STABLE
          counting sort.
   TIP  : Time = O(d * (n + 10)), where d = number of digits in the max
          number. Works for non-negative integers only (negatives need
          separate handling).
   ----------------------------------------------------------------------- */
void countingSortByDigit(vector<int>& a, int exp) {
    int n = a.size();
    vector<int> out(n), count(10, 0);

    for (int i = 0; i < n; i++) count[(a[i] / exp) % 10]++;   // frequency of the current digit
    for (int i = 1; i < 10; i++) count[i] += count[i - 1];    // prefix sum
    for (int i = n - 1; i >= 0; i--) {                        // from the back for stability
        int digit = (a[i] / exp) % 10;
        out[--count[digit]] = a[i];
    }
    a = out;
}

void radixSort(vector<int>& a) {
    if (a.empty()) return;
    int mx = *max_element(a.begin(), a.end());
    for (int exp = 1; mx / exp > 0; exp *= 10)   // 1, 10, 100, ...
        countingSortByDigit(a, exp);
}

/* -----------------------------------------------------------------------
   9) BUCKET SORT
   TRICK: "Make boxes, fill them, sort each box, join". Distribute values
          into buckets by range, sort each bucket (insertion/std sort), then
          concatenate all buckets in order.
   WHEN : When data is uniformly distributed (e.g., floats in [0,1)).
   ----------------------------------------------------------------------- */
void bucketSort(vector<int>& a) {
    if (a.empty()) return;
    int n = a.size();
    int mn = *min_element(a.begin(), a.end());
    int mx = *max_element(a.begin(), a.end());
    if (mn == mx) return;                        // all elements are equal

    vector<vector<int>> buckets(n);              // n buckets
    for (int x : a) {
        // map the value to a bucket index in [0, n-1]
        int idx = (long long)(x - mn) * (n - 1) / (mx - mn);
        buckets[idx].push_back(x);
    }
    int k = 0;
    for (auto& b : buckets) {
        sort(b.begin(), b.end());                // sort each bucket
        for (int x : b) a[k++] = x;              // write back into the main array
    }
}

/* -----------------------------------------------------------------------
   10) SHELL SORT  (improved Insertion sort)
   TRICK: "Insertion sort with a gap". First sort far-apart elements (big
          gap), then keep shrinking the gap. At the end gap = 1 (plain
          insertion sort), but by then the array is almost sorted.
   ----------------------------------------------------------------------- */
void shellSort(vector<int>& a) {
    int n = a.size();
    for (int gap = n / 2; gap > 0; gap /= 2) {   // gap sequence: n/2, n/4, ..., 1
        for (int i = gap; i < n; i++) {
            int temp = a[i];
            int j = i;
            while (j >= gap && a[j - gap] > temp) { // insertion sort, but with step = gap
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}

/* -----------------------------------------------------------------------
   11) STL sort  (what you use in CP)
   TRICK: Introsort = Quick sort + Heap sort (when recursion gets too deep)
          + Insertion sort (for small parts). Guaranteed O(n log n). Use
          stable_sort() if you need stability (it is merge sort based).
   ----------------------------------------------------------------------- */
void stlSort(vector<int>& a) {
    sort(a.begin(), a.end());                    // ascending
    // sort(a.rbegin(), a.rend());               // descending (reverse iterators)
    // sort(a.begin(), a.end(), greater<int>()); // descending (comparator)
    // sort(a.begin(), a.end(), [](int x, int y){ return x > y; }); // lambda
}

/* ----------------------------- TESTING --------------------------------- */
void printArr(const string& name, const vector<int>& a) {
    cout << left << setw(15) << name << ": ";
    for (int x : a) cout << x << " ";
    cout << "\n";
}

int main() {
    srand(time(nullptr));
    vector<int> original = {64, 25, 12, 22, 11, 90, 5, 25, 78, 1};
    printArr("Original", original);
    cout << "-----------------------------------------------\n";

    vector<int> a;

    a = original; bubbleSort(a);               printArr("Bubble", a);
    a = original; selectionSort(a);            printArr("Selection", a);
    a = original; insertionSort(a);            printArr("Insertion", a);
    a = original; mergeSort(a, 0, a.size()-1); printArr("Merge", a);
    a = original; quickSort(a, 0, a.size()-1); printArr("Quick", a);
    a = original; heapSort(a);                 printArr("Heap", a);
    a = original; countingSort(a);             printArr("Counting", a);
    a = original; radixSort(a);                printArr("Radix", a);
    a = original; bucketSort(a);               printArr("Bucket", a);
    a = original; shellSort(a);                printArr("Shell", a);
    a = original; stlSort(a);                  printArr("STL sort", a);

    return 0;
}
