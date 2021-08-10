#!/usr/bin/env python

"""
 Read feedback from a csv file. Integrate it in the current findings. Delete the file.
"""

import datetime
import os

import pandas as pd

from util import get_conf

LOG_COLUMNS = ["id", "values", "date"]

NOW = (datetime.datetime.now() - datetime.datetime(1970, 1, 1)).total_seconds()

def main(conf):
    ref_df = pd.read_csv(conf.reference_filename, sep=';')

    log_df = pd.DataFrame(columns=LOG_COLUMNS) if not os.path.isfile(
        conf.log_filename) else pd.read_csv(conf.log_filename, sep=';')

    df = ref_df.merge(log_df, on='id', how="left")

    df.to_csv(path_or_buf=conf.log_filename,
              columns=LOG_COLUMNS, index=False, sep=";")

if __name__ == "__main__":
    main(get_conf())
