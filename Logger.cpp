#ifdef LOGGER_IMPLEMENTATION

bool Logger::initialized = false;
bool Logger::console_output = true;
bool Logger::save_to_file = false;
LogLevel    Logger::priority_level = LogLevel::INFO;
std::string Logger::log_file_path = "LOG.txt";
std::mutex  Logger::logger_mutex;

Logger Logger::get_instance()
{
    static Logger instance;
    return instance;
}

void Logger::init(LogLevel priority_level, bool save_to_file, bool console_output, std::string log_file_path)
{
    if (console_output == false && save_to_file == false)
    {
        return;
    }

    if (save_to_file)
    {
        if (log_file_path != "")
        {
            get_instance().log_file_path = log_file_path;
        }
        get_instance().save_to_file = true;
    }


    get_instance().console_output = console_output;
    get_instance().priority_level = priority_level;
    get_instance().initialized = true;
}

void Logger::log(LogLevel log_level, std::string message)
{
    if (log_level >= get_instance().priority_level && get_instance().initialized)
    {
        logger_mutex.lock();
        bool time_format_avail = false;
        typedef std::chrono::system_clock clock;

        auto now = clock::now();
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto fraction = now - seconds;
        std::time_t cnow = clock::to_time_t(now);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);

        char time_str[100];
        if (std::strftime(time_str, sizeof(time_str), "%H:%M:%S:", std::localtime(&cnow)))
        {
            time_format_avail = true;
        }

        if (get_instance().console_output)
        {
            if (time_format_avail)
            {
                std::cout << time_str << milliseconds.count() << " ";
            }
            std::cout << message << std::endl;
        }

        if (get_instance().save_to_file)
        {
            std::ofstream file(log_file_path, std::ios_base::app);
            if (time_format_avail)
            {
                file << time_str << milliseconds.count() << " ";
            }
            file << message << std::endl;
            file.close();
        }
        logger_mutex.unlock();
    }
}

void Logger::Fatal(std::string message)
{
    log(LogLevel::FATAL, "[FATAL]\t" + message);
}

void Logger::Error(std::string message)
{
    log(LogLevel::ERROR, "[ERROR]\t" + message);
}

void Logger::Warn(std::string message)
{
    log(LogLevel::WARN, "[WARN] \t" + message);
}

void Logger::Info(std::string message)
{
    log(LogLevel::INFO, "[INFO] \t" + message);
}

void Logger::Debug(std::string message)
{
    log(LogLevel::DEBUG, "[DEBUG]\t" + message);
}

#endif