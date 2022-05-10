import pytest
from selenium import webdriver

driver = None

@pytest.fixture(scope='session')
def driver(request):
    global driver
    if driver is None:
        name = request.config.getoption("--browser")
        if name == "firefox":
            driver = webdriver.Firefox()
        elif name == "chrome":
            driver = webdriver.Chrome()
        else:
            driver = webdriver.Chrome()

    def fn():
        driver.quit()

    request.addfinalizer(fn)
    return driver
