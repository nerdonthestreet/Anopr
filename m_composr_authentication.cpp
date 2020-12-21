#include "module.h"
#include "modules/sql.h"

static Module *me;

class ModuleComposrAuthentication : public Module
{
	Anope::string engine;
	Anope::string query;
	Anope::string password_script, email_script, fallback_email, disable_reason, disable_email_reason;

  public:
	ModuleComposrAuthentication(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		me = this;
	}

	// Load in configuration from Anope's modules.conf file.
	void OnReload(Configuration::Conf *conf) anope_override
	{
		Configuration::Block *config = conf->GetModule(this);
		this->password_script = config->Get<const Anope::string>("password_script");
		this->email_script = config->Get<const Anope::string>("email_script");
		this->fallback_email = config->Get<const Anope::string>("fallback_email");
		this->disable_reason = config->Get<const Anope::string>("disable_reason");
		this->disable_email_reason = config->Get<const Anope::string>("disable_email_reason");
	}
	
	// Function to check if a Composr password is correct or not, given a Composr username and password.
	bool composrCheckPassword(Anope::string username, Anope::string password) {
		// Build the OS command.
		Anope::string commandBlank = "php ";
		Anope::string commandBlankTwo = commandBlank.append(password_script);
		Anope::string commandBlankThree = commandBlankTwo.append(" ");
		Anope::string commandBlankFour = commandBlankThree.append(username);
		Anope::string commandBlankFive = commandBlankFour.append(" ");
		Anope::string commandBlankSix = commandBlankFive.append(password);
		
		// Create a buffer to hold the command output.
		char buf[128];
		FILE *fp;
		
		// Attempt to open a pipe to get the PHP script output.
		if ((fp = popen(commandBlankSix.c_str(), "r")) == NULL) {
			Log(LOG_NORMAL) << "m_composr_authentication: Error opening pipe to PHP script (password).";
			return -1;
		}
		while (fgets(buf, 128, fp) != NULL) {
			// Work with the PHP output here.
			std::string outputString = buf; // Read the buffer into a string.
			
			// The PHP script used to have a newline at the end of its output (for CLI usage), which this code would remove.
			/* if (!outputString.empty() && outputString[outputString.length()-1] == '\n') {
				outputString.erase(outputString.length()-1);
			} */
			
			// Convert the string to a char.
			char* outputChar = &outputString[0];
			Log(LOG_NORMAL) << "m_composr_authentication: Output stored in memory: " << outputChar;
			fflush(stdout);
			
			// Check if output was true or not.
			if (strcmp(outputChar, "true") == 0) {
				Log(LOG_NORMAL) << "m_composr_authentication: Output was true, returning a successful authentication.";
				fflush(stdout);
				return true;
			} else {
				// Assume anything other than "true" was "false" (safest for failure/error messages.)
				Log(LOG_NORMAL) << "m_composr_authentication: Output was false, returning an unsuccessful authentication.";
				fflush(stdout);
				return false;
			}
		}
		if(pclose(fp))  {
			Log(LOG_NORMAL) << "m_composr_authentication: PHP file was missing or exited with an error status.";
			return false;
		}
		return false;
	}
	
	// Function to get the Composr email address of a user, given a Composr username.
	Anope::string composrGetEmail(Anope::string username) {
		// Build the OS command.
		Anope::string commandBlank = "php ";
		Anope::string commandBlankTwo = commandBlank.append(email_script);
		Anope::string commandBlankThree = commandBlankTwo.append(" ");
		Anope::string commandBlankFour = commandBlankThree.append(username);
		
		// Create a buffer to hold the command output.
		char buf[128];
		FILE *fp;
		
		// Attempt to open a pipe to get the PHP script output.
		if ((fp = popen(commandBlankThree.c_str(), "r")) == NULL) {
			Log(LOG_NORMAL) << "m_composr_authentication: Error opening pipe to PHP script (email).";
			return -1;
		}
		while (fgets(buf, 128, fp) != NULL) {
			// Work with PHP output here.
			std::string outputString = buf; // Read the buffer into a string.
			char* outputChar = &outputString[0];
			return outputChar;
		}
		if(pclose(fp))  {
			Log(LOG_NORMAL) << "m_composr_authentication: PHP file was missing or exited with an error status.";
			return fallback_email; // In case of failure, still populate the field so the user can chat.
		}
		return fallback_email; // In case of failure, still populate the field so the user can chat.
	}
	
	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_override
	{
		if (!this->disable_reason.empty() && (command->name == "nickserv/register" || command->name == "nickserv/group"))
		{
			source.Reply(this->disable_reason);
			return EVENT_STOP;
		}
		
		if (!this->disable_email_reason.empty() && command->name == "nickserv/set/email")
		{
			source.Reply(this->disable_email_reason);
			return EVENT_STOP;
		}
		
		return EVENT_CONTINUE;
	}
	
	void OnCheckAuthentication(User *u, IdentifyRequest *req) anope_override
	{
		// Check Composr credentials against our PHP script.
		Anope::string username = req->GetAccount();
		Anope::string password = req->GetPassword();
		Anope::string output;
		Reference<User> user;
		
		Log(LOG_NORMAL) << "m_composr_authentication: User is trying to authenticate: " << username;
		fflush(stdout);
		
		bool successful = composrCheckPassword(username, password);
		
		Log(LOG_NORMAL) << "m_composr_authentication: The authentication request has completed.";
		fflush(stdout);
		
		if (successful == true) {
			Log(LOG_NORMAL) << "m_composr_authentication: Authentication was successful, returning success for request.";
			fflush(stdout);
			
			NickAlias *na = NickAlias::Find(req->GetAccount());
			BotInfo *NickServ = Config->GetClient("NickServ");
			// Create the nickname on the IRC server, if it's never been logged into here before.
			if (na == NULL) {
				na = new NickAlias(req->GetAccount(), new NickCore(req->GetAccount()));
				FOREACH_MOD(OnNickRegister, (user, na, ""));
				if (user && NickServ)
				user->SendMessage(NickServ, _("Your NOTS account \002%s\002 has been added to IRC."), na->nick.c_str());
			}
			
			// Copy the email address from the NOTS account to the IRC account, if it's not already up-to-date.
			Anope::string email = composrGetEmail(username);
			if (!email.empty() && email != na->nc->email) {
				na->nc->email = email;
				// The next two lines can be un-commented to notify users when their Composr email has synced to IRC.
				// if (user && NickServ)
				// user->SendMessage(NickServ, _("Your email has been updated to \002%s\002."), email.c_str());
				Log(LOG_NORMAL) << "m_composr_authentication: Updated email address to " << email.c_str();
			}
			
			// Return the success to the server.
			req->Success(me);
			Log(LOG_NORMAL) << "m_composr_authentication: Returned a success for user: " << username;
			fflush(stdout);
			return;
		} // Otherwise, return failure to the server.
		Log(LOG_NORMAL) << "m_composr_authentication: Authentication was not successful, not returning anything for user: " << username;
		fflush(stdout);
		return;
	}
};

MODULE_INIT(ModuleComposrAuthentication)
