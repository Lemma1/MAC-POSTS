# -*- coding: utf-8 -*-

region_code_map = {
    'aichi': 'JP-23',
    'akita': 'JP-05',
    'aomori': 'JP-02',
    'chiba': 'JP-12',
    'ehime': 'JP-38',
    'fukui': 'JP-18',
    'fukuoka': 'JP-40',
    'fukushima': 'JP-07',
    'gifu': 'JP-21',
    'gunma': 'JP-10',
    'hiroshima': 'JP-34',
    'hokkaido': 'JP-01',
    'hyogo': 'JP-28',
    'ibaraki': 'JP-08',
    'ishikawa': 'JP-17',
    'iwate': 'JP-03',
    'kagawa': 'JP-37',
    'kagoshima': 'JP-46',
    'kanagawa': 'JP-14',
    'kochi': 'JP-39',
    'kumamoto': 'JP-43',
    'kyoto': 'JP-43',
    'mie': 'JP-24',
    'miyagi': 'JP-04',
    'miyazaki': 'JP-45',
    'nagano': 'JP-20',
    'nagasaki': 'JP-42',
    'nara': 'JP-29',
    'niigata': 'JP-15',
    'oita': 'JP-44',
    'okayama': 'JP-33',
    'okinawa': 'JP-47',
    'osaka': 'JP-27',
    'saga': 'JP-41',
    'saitama': 'JP-11',
    'shiga': 'JP-25',
    'shimane': 'JP-32',
    'shizuoka': 'JP-22',
    'tochigi': 'JP-09',
    'tokushima': 'JP-36',
    'tokyo': 'JP-13',
    'tottori': 'JP-31',
    'toyama': 'JP-16',
    'wakayama': 'JP-30',
    'yamagata': 'JP-06',
    'yamaguchi': 'JP-35',
    'yamanashi': 'JP-19',
}


def converter(region_name):
    name = region_name.lower()
    if name in region_code_map:
        return region_code_map[name]
    else:
        return region_name
