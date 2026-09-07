"""
Additional Practice Tasks
=========================
1. Identity Matrix
2. Array Normalization
3. Prime Numbers with np.vectorize
4. Stacking Arrays
5. Cumulative Operations
"""

import numpy as np


# ─────────────────────────────────────────────────────
#  1. Identity Matrix
# ─────────────────────────────────────────────────────
def practice_1_identity_matrix():
    print("=" * 60)
    print("  PRACTICE 1: Identity Matrix")
    print("=" * 60)

    I = np.eye(4)
    print("4×4 Identity Matrix (I):\n", I)

    # Create an arbitrary 4×4 matrix
    M = np.random.randint(1, 10, size=(4, 4))
    print("\nRandom 4×4 Matrix (M):\n", M)

    # Verify I @ M == M and M @ I == M
    left = I @ M
    right = M @ I
    print("\nI @ M:\n", left)
    print("M @ I:\n", right)
    print(f"\nI @ M == M ? {np.array_equal(left, M)}")
    print(f"M @ I == M ? {np.array_equal(right, M)}")


# ─────────────────────────────────────────────────────
#  2. Array Normalization (min-max to [0, 1])
# ─────────────────────────────────────────────────────
def practice_2_normalization():
    print("\n" + "=" * 60)
    print("  PRACTICE 2: Array Normalization")
    print("=" * 60)

    arr = np.array([10.0, 25.0, 8.0, 42.0, 15.0, 30.0])
    print(f"Original array: {arr}")

    # Vectorized min-max normalization: (x - min) / (max - min)
    normalized = (arr - arr.min()) / (arr.max() - arr.min())
    print(f"Normalized [0,1]: {normalized}")
    print(f"Min: {normalized.min():.1f}, Max: {normalized.max():.1f}")


# ─────────────────────────────────────────────────────
#  3. Prime Numbers using np.vectorize
# ─────────────────────────────────────────────────────
def practice_3_primes():
    print("\n" + "=" * 60)
    print("  PRACTICE 3: Prime Numbers with np.vectorize")
    print("=" * 60)

    def is_prime(n):
        if n < 2:
            return False
        for i in range(2, int(n**0.5) + 1):
            if n % i == 0:
                return False
        return True

    vectorized_prime = np.vectorize(is_prime)

    arr = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 17, 20, 23, 29, 31])
    print(f"Input array: {arr}")

    prime_mask = vectorized_prime(arr)
    primes = arr[prime_mask]

    print(f"Boolean mask: {prime_mask}")
    print(f"Prime numbers found: {primes}")


# ─────────────────────────────────────────────────────
#  4. Stacking Arrays (vstack & hstack)
# ─────────────────────────────────────────────────────
def practice_4_stacking():
    print("\n" + "=" * 60)
    print("  PRACTICE 4: Stacking Arrays")
    print("=" * 60)

    a = np.array([1, 2, 3, 4])
    b = np.array([5, 6, 7, 8])

    print(f"Array a: {a}")
    print(f"Array b: {b}")

    vertical = np.vstack((a, b))
    horizontal = np.hstack((a, b))

    print(f"\nVertical stack (np.vstack):\n{vertical}")
    print(f"\nHorizontal stack (np.hstack):\n{horizontal}")


# ─────────────────────────────────────────────────────
#  5. Cumulative Operations (cumsum & cumprod)
# ─────────────────────────────────────────────────────
def practice_5_cumulative():
    print("\n" + "=" * 60)
    print("  PRACTICE 5: Cumulative Operations")
    print("=" * 60)

    arr = np.array([1, 2, 3, 4, 5])
    print(f"Original array: {arr}")

    cum_sum = np.cumsum(arr)
    cum_prod = np.cumprod(arr)

    print(f"Cumulative Sum (np.cumsum):  {cum_sum}")
    print(f"Cumulative Prod (np.cumprod): {cum_prod}")


# ─────────────────────────────────────────────────────
#  Run all practice tasks
# ─────────────────────────────────────────────────────
if __name__ == "__main__":
    practice_1_identity_matrix()
    practice_2_normalization()
    practice_3_primes()
    practice_4_stacking()
    practice_5_cumulative()
