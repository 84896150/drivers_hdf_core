import pytest
from selenium import webdriver

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
        print("Name of the browser that is launching：{name}")

    def fn():
        driver.quit()

    request.addfinalizer(fn)
    return driver
