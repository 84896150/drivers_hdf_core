import pytest
from selenium import webdriver

_driver = None

def pytest_addoption(parser):
    parser.addoption(
        "--browser", action="store", default="chrome", help="browser option: firefox or chrome"
             )

@pytest.fixture(scope='session')
def browser(request):
    global _driver
    if _driver is None:
        name = request.config.getoption("--browser")
        if name == "firefox":
            _driver = webdriver.Firefox()
        elif name == "chrome":
            _driver = webdriver.Chrome()
        else:
            _driver = webdriver.Firefox()

    def fn():
        _driver.quit()

    request.addfinalizer(fn)
    return _driver
