#!/usr/bin/env python

"""
 Create a new file for the next session based on the log.
"""

from datetime import date

import pandas as pd
from pandas.core.dtypes.missing import isna

from algo_sm2 import supermemo_2
from util import get_conf

OUTPUT_COLUMNS = ["id", "label", "l1", "v1",
                  "l2", "v2", "l3", "v3", "l4", "v4", "goal"]

NOW = date.today()

# https://note.nkmk.me/en/python-check-int-float/
def is_integer(n):
    try:
        float(n)
    except ValueError:
        return False
    else:
        return float(n).is_integer()


def to_string(x):
    return str(int(x)) if is_integer(x) else x

def main(conf):
    df = pd.read_csv(conf.reference_filename, sep=';')

    df["v1"] = df["v1"].apply(to_string)
    df["v2"] = df["v2"].apply(to_string)
    df["v3"] = df["v3"].apply(to_string)
    df["v4"] = df["v4"].apply(to_string)

    df.head(20).to_csv(path_or_buf=conf.next_session_filename,
                       columns=OUTPUT_COLUMNS, index=False, sep=";")


if __name__ == "__main__":
    main(get_conf())
