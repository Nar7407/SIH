"""AI Case Study: Patient Test-Result Analysis using Pandas DataFrame - Task 1"""

import pandas as pd

df = pd.read_csv("patient_tests.csv")
print("First 5 records of the patient tests dataset:")
print(df.head())
