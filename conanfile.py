from conans import python_requires

common = python_requires('conan_common_recipes/0.0.8@Manu343726/testing')

class ReactiveTelegram(common.CMakePackage):
    name = 'reactive-telegram'
    version = '0.0.0'
    url = 'https://github.com/joaquintides/usingstdcpp2019'
    license = 'MIT'
    requires = ('tgbot_cpp/1.1@Manu343726/testing',
                'boost_signals2/1.69.0@bincrafters/testing',
                'fmt/5.2.1@bincrafters/stable',
                'range-v3/0.4.0@ericniebler/stable')
    default_options = {'tgbot_cpp:shared': False}
    generators = 'cmake'
