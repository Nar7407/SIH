"""
AI Case Study: Abnormal-Glucose Detection using NumPy Arrays
============================================================
Core program — vectorized threshold comparison and Boolean indexing.
"""

import numpy as np


def main():
    # --- Create arrays ---
    glucose = np.array([110.0, 165.0, 185.5, 95.0, 150.0])
    patient_ids = np.array(["H101", "H102", "H103", "H104", "H105"])

    # --- Define threshold ---
    threshold = 140  # mg/dL

    # --- Vectorized threshold comparison (produces Boolean array) ---
    abnormal_flag = glucose > threshold

    # --- Display results ---
    print(f"Glucose Reading Array:\n{glucose}")
    print(f"\nAbnormal Flags:\n{abnormal_flag}")

    # --- Boolean indexing to extract abnormal patients ---
    abnormal_patients = patient_ids[abnormal_flag]
    abnormal_glucose = glucose[abnormal_flag]

    print(f"\nAbnormal-Glucose Patients:")
    for pid in abnormal_patients:
        print(f"  {pid}")

    print(f"\nGlucose Readings of Abnormal Samples:\n{abnormal_glucose}")

    # --- Statistics ---
    num_abnormal = int(np.sum(abnormal_flag))
    total_patients = len(patient_ids)
    abnormal_pct = (num_abnormal / total_patients) * 100

    print(f"\nTotal Abnormal Samples: {num_abnormal}")
    print(f"Total Patients: {total_patients}")
    print(f"Abnormal Percentage: {abnormal_pct:.2f}%")


if __name__ == "__main__":
    main()
