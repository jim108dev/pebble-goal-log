#!/usr/bin/env python

"""
 Create a new file for the next session based on the log.
"""

from datetime import date

import pandas as pd
from pandas.core.dtypes.missing import isna

from algo_sm2 import supermemo_2
from util import get_conf

OUTPUT_COLUMNS = ["id", "label", "labels", "values", "goal"]

NOW = date.today()

def countdown(last_stop, waiting_period):
    dt_last_stop = date.fromtimestamp(int(last_stop))
    delta = NOW - dt_last_stop
    days_past = delta.days
    return waiting_period - days_past


def main(conf):
    df = pd.read_csv(conf.reference_filename, sep=';')

    df.head(20).to_csv(path_or_buf=conf.next_session_filename, columns=OUTPUT_COLUMNS, index=False, sep=";")

if __name__ == "__main__":
    main(get_conf())
