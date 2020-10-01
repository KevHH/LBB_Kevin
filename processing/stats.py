
from datetime import date, datetime

birth_date = date(2020, 9, 14)

def get_age():
    today = datetime.now().date()
    delta = today - birth_date
    return str(delta.days) + " days" 