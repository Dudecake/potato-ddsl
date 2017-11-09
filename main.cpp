#include <unistd.h>
#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/mdc.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/properties.h>
#include <boost/filesystem.hpp>

#include "cxxopts.hpp"
#include "program.h"

//namespace po = boost::program_options;

inline bool endsWith(const std::string &value, const std::string &suffix)
{
    if (suffix.size() > value.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), value.rbegin());
}

int main(int argc, char *argv[])
{
    cxxopts::Options options(argv[0]);
    options.positional_help("<dataset root>");
    bool error = false;
    bool needHelp = false;
    bool verbose = false;
    std::string seed;
    std::string modelDir;
    std::string logfile;
    std::vector<std::string> args;
    std::string dataRoot;
    options.add_options()
            ("h, help", "Get help message", cxxopts::value<bool>(needHelp))
            ("m, model", "Specify model and solver directory", cxxopts::value<std::string>(modelDir))
            ("l, logfile", "Log to file", cxxopts::value<std::string>(logfile))
            ("v, verbose", "Specify verbosity, up to -vvv", cxxopts::value<bool>(verbose))
            ("dataroot", "Location of dataset", cxxopts::value<std::vector<std::string>>(args));
    options.parse_positional("dataroot");
    options.parse(argc, argv);
    std::hash<std::string>();
    srand(static_cast<uint>(time(nullptr)));

    if (modelDir.empty())
    {
        std::cout << "Need to supply model directory" << std::endl;
        needHelp = error = true;
    }
    if (args.empty())
    {
        std::cout << "Need to supply dataset root" << std::endl;
        needHelp = error = true;
    }
    if (needHelp)
    {
        std::cout << options.help() << std::endl;
        return error;
    }

    if (seed.empty())
    {
        seed = "dinges";
    }
    srand(static_cast<uint>(std::hash<std::string>{}(seed)));

    std::stringstream ss;
    std::string loggingLevel = "INFO";
    std::string loggingTarget = "stdout";
    if (verbose)
    {
        switch (options.count("verbose"))
        {
            case 1:
                ss << "INFO, ";
                break;
            case 2:
                ss << "DEBUG, ";
                break;
            case 3:
                ss << "TRACE, ";
                break;
            default:
                std::cerr << "Invalid logging level specified, setting TRACE" << std::endl;
                ss << "TRACE, ";
                break;
        }
    }
    else
    {
        ss << "WARN, ";
    }
    if (!logfile.empty())
    {
        std::cout << "Logging to a file is not yet implemented" << std::endl;
        ss << "stdout";
    }
    else
    {
        ss << "stdout";
    }
    //dataRoot = options["dataroot"].as<std::vector<std::string>>();
    dataRoot = args.front();

    log4cxx::MDC::put("pid", std::to_string(getpid()));
    log4cxx::helpers::Properties properties;
    properties.put("log4j.rootCategory", ss.str());
    properties.put("log4j.appender.stdout", "org.apache.log4j.ConsoleAppender");
    properties.put("log4j.appender.stdout.layout", "org.apache.log4j.PatternLayout");;
    //properties.put("log4j.appender.stdout.layout.ConversionPattern", "%clr(%d{yyyy-MM-dd HH:mm:ss,SSS}){faint} %clr(%5p) %clr( ){magenta} %clr(---){faint} %clr([%15.15t]){faint} %clr(%-40.40logger{39}){cyan} %clr(:){faint} %m%n");
    properties.put("log4j.appender.stdout.layout.ConversionPattern", "\u001b[0;2m%d{yyyy-MM-dd HH:mm:ss.SSS}\u001b[m %5p  %X{pid} \u001b[0;2m---\u001b[m \u001b[0;2m[%14t]\u001b[m \u001b[0;36m%-16.16logger{39}\u001b[m \u001b[0;2m:\u001b[m %m%n");
    //properties.put("log4j.appender.stdout.layout.ConversionPattern", "\u001b[0;2m%d{yyyy-MM-dd HH:mm:ss.SSS}\u001b[m %5p  %X{pid} \u001b[0;2m---\u001b[m \u001b[0;2m[%14t]\u001b[m \u001b[0;36m%-32.32F:%L{39}\u001b[m \u001b[0;2m:\u001b[m %m%n");
    log4cxx::PropertyConfigurator::configure(properties);
    log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("Main");

    namespace fs = boost::filesystem;
    if (dataRoot.at(0) != '/')
        dataRoot = fs::absolute(dataRoot).string();
    if (!fs::is_directory(modelDir))
    {
        LOG4CXX_ERROR(logger, "Model directory doesn't exists or isn't a directory");
        return 2;
    }
    boost::system::error_code fileError;
    fs::current_path(modelDir, fileError);
    if (fileError.value() != 0)
    {
        LOG4CXX_ERROR(logger, "Could not change directory: " << fileError.message());
        return 2;
    }
    fs::directory_iterator end_iter;
    std::string suffix(".prototxt");
    std::string modelName;
    std::string solverName;
    for (fs::directory_iterator iter(modelDir); iter != end_iter; iter++)
    {
        std::string filename = iter->path().filename().string();
        bool isPrototxt = endsWith(filename, suffix);
        if (fs::is_regular_file(iter->status()) && isPrototxt)
        {
            std::transform(filename.begin(), filename.end(), filename.begin(), [](unsigned char c){ return std::tolower(c); });
            if (filename.find("model") != std::string::npos)
            {
                modelName = iter->path().filename().string();
                LOG4CXX_INFO(logger, "Found model " << iter->path().string());
            }
            else if (filename.find("solver") != std::string::npos)
            {
                solverName = iter->path().filename().string();
                LOG4CXX_INFO(logger, "Found model " << iter->path().string());

            }
        }
    }
    if (modelName.empty() || solverName.empty())
    {
        LOG4CXX_ERROR(logger, "Directory \"" << modelDir << "\" doesn't contain model or solver");
        return 3;
    }

    Program p(dataRoot, modelName, solverName);
    return p.run();
}
