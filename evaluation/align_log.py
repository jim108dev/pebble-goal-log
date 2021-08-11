#!/usr/bin/env python

"""
 Read feedback from a csv file. Integrate it in the current findings. Delete the file.
"""

import datetime
import os

import pandas as pd

from util import get_conf

LOG_COLUMNS = ["id", "v1","v2","v3","v4", "date"]

NOW = (datetime.datetime.now() - datetime.datetime(1970, 1, 1)).total_seconds()

def main(conf):
    if os.path.isfile(conf.log_filename):
        print("Log file exists. Bailing out!")
        exit()

    df = pd.DataFrame(columns=LOG_COLUMNS)

    df.to_csv(path_or_buf=conf.log_filename,
              columns=LOG_COLUMNS, index=False, sep=";")

if __name__ == "__main__":
    main(get_conf())
