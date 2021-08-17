#!/usr/bin/env python

"""
 Create a new file for the next session based on the log.
"""

import logging
from datetime import date, datetime

import pandas as pd
from pandas.core.dtypes.missing import isna

from util import get_conf

OUTPUT_COLUMNS = ["id", "label", "l1", "v1",
                  "l2", "v2", "l3", "v3", "l4", "v4", "goal"]

NOW = datetime.today()

# https://note.nkmk.me/en/python-check-int-float/
def is_integer(n):
    try:
        float(n)
    except ValueError:
        return False
    else:
        return float(n).is_integer()

def countdown(last_processed, waiting_period):
    delta = NOW - last_processed
    days_past = delta.days
    return waiting_period - days_past

def to_string(x):
    return str(int(x)) if is_integer(x) else x

def main(conf):
    r_df = pd.read_csv(conf.reference_filename, sep=';')

    r_df["start"] = pd.to_datetime(r_df['start'].astype('datetime64[ns]'), unit='s')
    r_df = r_df[r_df["start"]<=NOW]

    r_df["stop"] = pd.to_datetime(r_df['stop'].astype('datetime64[ns]'), unit='s')
    r_df = r_df[r_df["stop"]>=NOW]

    l_df = pd.read_csv(conf.log_filename, sep=';')

    l_df.sort_values('date').drop_duplicates(['id'])

    df = r_df.merge(l_df, how="left")

    df["v1"] = df["v1"].apply(to_string)
    df["v2"] = df["v2"].apply(to_string)
    df["v3"] = df["v3"].apply(to_string)
    df["v4"] = df["v4"].apply(to_string)

    df["date"].fillna(0,inplace=True)
    df["last_processed"] = pd.to_datetime(df['date'].astype('datetime64[ns]'), unit='s')
    

    df['countdown'] = df.apply(lambda x: 0 if isna(x['last_processed']) else countdown(
        x['last_processed'], x['step']), axis=1)

    logging.debug(df.head())
    
    df.to_csv(path_or_buf=conf.next_session_filename,
                       columns=OUTPUT_COLUMNS, index=False, sep=";")


if __name__ == "__main__":
    main(get_conf())
