#!/usr/bin/env python

"""
 Read feedback from a csv file. Integrate it in the current findings. Delete the file.
"""

import os
import os.path
import shutil
import tempfile
from datetime import datetime

import pandas as pd

import logging

OUTPUT_COLUMNS = ["id", "v1","v2","v3","v4", "date"]


def main(conf):
    if not os.path.isfile(conf.feedback_filename):
        print("%s does not exist" % conf.feedback_filename)
        return

    f_df = pd.read_csv(conf.feedback_filename, sep=';')
    l_df = pd.read_csv(conf.log_filename, sep=';')


    f_df["date_ms"] = f_df["date"].apply(datetime.fromtimestamp)
    f_df["date"] = pd.to_datetime(f_df['date_ms'].astype('datetime64[ns]'), unit='s', format="%y-%m%d")
    f_df["date"] = f_df["date"].dt.date

    df = pd.concat([l_df, f_df])

    logging.debug(df.head())

    df.to_csv(path_or_buf=conf.log_filename,
              columns=OUTPUT_COLUMNS, sep=";", mode='a', header=False)

    #os.remove(conf.feedback_filename)

if __name__ == "__main__":
    main(get_conf())
