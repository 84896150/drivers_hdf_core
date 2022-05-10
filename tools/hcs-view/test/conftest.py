import pytest
from selenium import webdriver
global driver

def pytest_addoption(parser):
    parser.addoption(
        "--browser", action="store", default="chrome", help="browser option: firefox or chrome"
             )

@pytest.fixture(scope='session')
def driver(request):
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
