#include "Webserv.hpp"

// Constructor
CgiHandler::CgiHandler() {
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
}

CgiHandler::CgiHandler(std::string path)
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = path;
	this->_ch_env = NULL;
	this->_argv = NULL;
}

// Destructor
CgiHandler::~CgiHandler() {

	if (this->_ch_env)
	{
		for (int i = 0; this->_ch_env[i]; i++)
			free(this->_ch_env[i]);
		free(this->_ch_env);
	}
	if (this->_argv)
	{
		for (int i = 0; this->_argv[i]; i++)
			free(_argv[i]);
		free(_argv);
	}
	this->_env.clear();
}

CgiHandler::CgiHandler(const CgiHandler &other)
{
		this->_env = other._env;
		this->_ch_env = other._ch_env;
		this->_argv = other._argv;
		this->_cgi_path = other._cgi_path;
		this->_cgi_pid = other._cgi_pid;
		this->_exit_status = other._exit_status;
}

CgiHandler &CgiHandler::operator=(const CgiHandler &rhs)
{
    if (this != &rhs)
	{
		this->_env = rhs._env;
		this->_ch_env = rhs._ch_env;
		this->_argv = rhs._argv;
		this->_cgi_path = rhs._cgi_path;
		this->_cgi_pid = rhs._cgi_pid;
		this->_exit_status = rhs._exit_status;
	}
	return (*this);
}

/* Setters */
void CgiHandler::setCgiPid(pid_t cgi_pid)
{
    this->_cgi_pid = cgi_pid;
}

void CgiHandler::setCgiPath(const std::string &cgi_path)
{
	this->_cgi_path = cgi_path;
}

/* Getters */
const std::map<std::string, std::string> &CgiHandler::getEnv() const
{
    return (this->_env);
}

const pid_t &CgiHandler::getCgiPid() const
{
    return (this->_cgi_pid);
}

const std::string &CgiHandler::getCgiPath() const
{
    return (this->_cgi_path);
}

void CgiHandler::initEnvCgi(HTTP::Request& req, const std::vector<Location>::iterator it_loc) {
    std::string cgi_exec = ("cgi-bin/" + it_loc->getCgiPath()[0]).c_str();
	char    *cwd = getcwd(NULL, 0);
	if(_cgi_path[0] != '/')
	{
		std::string tmp(cwd);
		tmp.append("/");
		if(_cgi_path.length() > 0)
			_cgi_path.insert(0, tmp);
	}
	if(req.getMethod() == HTTP::POST)
	{
		std::stringstream out;
		out << req.getBody().length();
		this->_env["CONTENT_LENGTH"] = out.str();
		this->_env["CONTENT_TYPE"] = req.getHeader("content-type");
	}
    this->_env["GATEWAY_INTERFACE"] = std::string("CGI/1.1");
	this->_env["SCRIPT_NAME"] = cgi_exec;//
    this->_env["SCRIPT_FILENAME"] = this->_cgi_path;
    this->_env["PATH_INFO"] = this->_cgi_path;//
    this->_env["PATH_TRANSLATED"] = this->_cgi_path;//
    this->_env["REQUEST_URI"] = this->_cgi_path;//
    this->_env["SERVER_NAME"] = req.getHeader("host");
    this->_env["SERVER_PORT"] ="8002";
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";

    std::map<std::string, std::string> request_headers = req.getHeaders();
	for(std::map<std::string, std::string>::iterator it = request_headers.begin();
		it != request_headers.end(); ++it)
	{
		std::string name = it->first;
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		std::string key = "HTTP_" + name;
		_env[key] = it->second;
	}
	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(cgi_exec.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}

/* initialization environment variable */
void CgiHandler::initEnv(HTTP::Request& req, const std::vector<Location>::iterator it_loc)
{
	int			poz;
	std::string extension;
	std::string ext_path;

	extension = this->_cgi_path.substr(this->_cgi_path.find("."));
	std::map<std::string, std::string>::iterator it_path = it_loc->_ext_path.find(extension);
    if (it_path == it_loc->_ext_path.end())
        return ;
    ext_path = it_loc->_ext_path[extension];

	this->_env["AUTH_TYPE"] = "Basic";
	this->_env["CONTENT_LENGTH"] = req.getHeader("content-length");
	this->_env["CONTENT_TYPE"] = req.getHeader("content-type");
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	poz = findStart(this->_cgi_path, "cgi-bin/");
	this->_env["SCRIPT_NAME"] = this->_cgi_path;
    this->_env["SCRIPT_FILENAME"] = ((poz < 0 || (size_t)(poz + 8) > this->_cgi_path.size()) ? "" : this->_cgi_path.substr(poz + 8, this->_cgi_path.size())); // check dif cases after put right parametr from the response
    this->_env["PATH_INFO"] = getPathInfo(req.getPath(), it_loc->getCgiExtension());
    this->_env["PATH_TRANSLATED"] = it_loc->getRootLocation() + (this->_env["PATH_INFO"] == "" ? "/" : this->_env["PATH_INFO"]);
    this->_env["QUERY_STRING"] = decode(req.getQuery());
    this->_env["REMOTE_ADDR"] = req.getHeader("host");
	poz = findStart(req.getHeader("host"), ":");
    this->_env["SERVER_NAME"] = (poz > 0 ? req.getHeader("host").substr(0, poz) : "");
    this->_env["SERVER_PORT"] = (poz > 0 ? req.getHeader("host").substr(poz + 1, req.getHeader("host").size()) : "");
    this->_env["REQUEST_METHOD"] = req.getMethodStr();
    this->_env["HTTP_COOKIE"] = req.getHeader("cookie");
    this->_env["DOCUMENT_ROOT"] = it_loc->getRootLocation();
	this->_env["REQUEST_URI"] = req.getPath() + req.getQuery();
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";

	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(ext_path.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}

/* Pipe and execute CGI */
void CgiHandler::execute(short &error_code)
{
	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		error_code = 500;
		return ;
	}
	if (pipe(pipe_in) < 0)
	{
        Logger::logMsg(RED, CONSOLE_OUTPUT, "pipe() failed");

		error_code = 500;
		return ;
	}
	if (pipe(pipe_out) < 0)
	{
        Logger::logMsg(RED, CONSOLE_OUTPUT, "pipe() failed");

		close(pipe_in[0]);
		close(pipe_in[1]);
		error_code = 500;
		return ;
	}
	this->_cgi_pid = fork();
	if (this->_cgi_pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);
		exit(this->_exit_status);
	}
	else if (this->_cgi_pid > 0){}
	else
	{
        std::cout << "Fork failed" << std::endl;
		error_code = 500;
	}
}

int CgiHandler::findStart(const std::string path, const std::string delim)
{
	if (path.empty())
		return (-1);
	size_t poz = path.find(delim);
	if (poz != std::string::npos)
		return (poz);
	else
		return (-1);
}

/* Translation of parameters for QUERY_STRING environment variable */
std::string CgiHandler::decode(std::string &path)
{
	size_t token = path.find("%");
	while (token != std::string::npos)
	{
		if (path.length() < token + 2)
			break ;
		char decimal = utils::hexToDec(path.substr(token + 1, 2));
		path.replace(token, 3, utils::toString(decimal));
		token = path.find("%");
	}
	return (path);
}

/* Isolation PATH_INFO environment variable */
std::string CgiHandler::getPathInfo(std::string& path, std::vector<std::string> extensions)
{
	std::string tmp;
	size_t start, end;

	for (std::vector<std::string>::iterator it_ext = extensions.begin(); it_ext != extensions.end(); it_ext++)
	{
		start = path.find(*it_ext);
		if (start != std::string::npos)
			break ;
	}
	if (start == std::string::npos)
		return "";
	if (start + 3 >= path.size())
		return "";
	tmp = path.substr(start + 3, path.size());
	if (!tmp[0] || tmp[0] != '/')
		return "";
	end = tmp.find("?");
	return (end == std::string::npos ? tmp : tmp.substr(0, end));
}

void		CgiHandler::clear()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
	this->_env.clear();
}

// void CgiHandler::initCgi() {
//     std::string file = _resource.find_last_of('.') != std::string::npos ? _resource.substr(_resource.find_last_of('/') + 1) :  _location.getIndexLocation();
//     std::string cgiScriptPath = _location.getRootLocation() + "/cgi-bin/" + file;
//     _resource = cgiScriptPath;
//     if (!cgiScriptExists(cgiScriptPath)) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "CGI script not found: %s", cgiScriptPath.c_str());
//         sendErrorResponse(404, "Not Found", "CGI script not found");
//         return;
//     }
//     if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "pipe error: %s", strerror(errno));
//         return;
//     }
//     _client->addFdToMonitor(_pipeOut[0], EPOLLIN);
//     _serverSocket->getCgiPipeMap()[_pipeOut[0]] = this;  // Add to _cgiPipeMap
//     initEnv();
//      std::vector<char *> envp = createEnvArray();
//     std::vector<char *> args;

//     // Construct the full path to the script
//     args.push_back(strdup(cgiScriptPath.c_str()));
//     args.push_back(0);
//     for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
//         Logger::logMsg("INFO", CONSOLE_OUTPUT, "env: %s=%s", it->first.c_str(), it->second.c_str());
//     }
//     _pid = fork();
//     if (_pid == -1) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "fork error: %s", strerror(errno));
//         sendErrorResponse(500, "Internal Server Error", "CGI script error");
//         return;
//     }
//     if (_pid == 0) {
//         // Child process
//         Logger::logMsg("INFO", CONSOLE_OUTPUT, "Forked child process for CGI");
//         close(_pipeIn[1]);
//         close(_pipeOut[0]);
//         dup2(_pipeIn[0], STDIN_FILENO);
//         dup2(_pipeOut[1], STDOUT_FILENO);
//         close(_pipeIn[0]);
//         close(_pipeOut[1]);
//         execve(args[0], &args[0], &envp[0]);
//         perror("execve");
//         exit(1);
//     } else {
//         // Parent process
//         Logger::logMsg("INFO", CONSOLE_OUTPUT, "Parent process continuing after fork");
//         close(_pipeIn[0]);
//         close(_pipeOut[1]);
//         setState(WRITING); // Set the state to WRITING after forking
//         _serverSocket->modifyEpoll(_pipeOut[0], EPOLLIN); // Update epoll to listen for reading events
//     }
// }

// void CgiHandler::readCgiResponse(Client &client, CgiHandler &cgi) {
//     char buffer[BUFF_SIZE * 2];
//     ssize_t bytesRead = read(cgi.pipeOut[0], buffer, BUFF_SIZE * 2);
//     std::string completeResponse;

//     while ((bytesRead = read(_pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
//         buffer[bytesRead] = '\0';
//         completeResponse += std::string(buffer, bytesRead);
//     }
//     if (bytesRead == -1 && errno != EAGAIN) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "read error: %s", strerror(errno));
//     }

//     int status;
//     if (waitpid(_pid, &status, WNOHANG) == -1) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "waitpid error: %s", strerror(errno));
//     } else if (WIFEXITED(status)) {
//         _exitStatus = WEXITSTATUS(status);
//         Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "Child process exited with status: %d", _exitStatus);
//     } else if (WIFSIGNALED(status)) {
//         Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "Child process was terminated by signal: %d", WTERMSIG(status));
//     }

//     if (_exitStatus == 0) {
//         std::string cgiResponse;
//         if (completeResponse.find("HTTP/1.1") == std::string::npos) {
//             cgiResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + completeResponse;
//         } else {
//             cgiResponse = completeResponse;
//         }
//         _client->addResponse(cgiResponse);
//     } else {
//         _client->addResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\nCGI script error");
//     }

//     Logger::logMsg(PURPLE, CONSOLE_OUTPUT, "execCgi completed with response: %s", completeResponse.c_str());
//     _state = DONE; // Set the state to DONE after executing CGI
// }

// void CgiHandler::sendCgiBody() {
//     const std::string requestBody = _req.getBody();
//     ssize_t bytesSent = write(_pipeIn[1], requestBody.c_str(), requestBody.size());
//     if (bytesSent == -1) {
//         Logger::logMsg(RED, CONSOLE_OUTPUT, "write error: %s", strerror(errno));
//         close(_pipeIn[1]);
//         return;
//     }
//     close(_pipeIn[1]);
//     setState(READING);
//     _serverSocket->modifyEpoll(_pipeOut[0], EPOLLIN);
// }

// void CgiHandler::closePipes() {
//     Logger::logMsg("INFO", CONSOLE_OUTPUT, "Closing pipes");
//     if (_pipeIn[0] != -1) close(_pipeIn[0]);
//     if (_pipeIn[1] != -1) close(_pipeIn[1]);
//     if (_pipeOut[0] != -1) close(_pipeOut[0]);
//     if (_pipeOut[1] != -1) close(_pipeOut[1]);
// }
