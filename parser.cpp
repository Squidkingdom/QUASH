#include "parser.h"
#include "built-ins.h"

/// @brief Tokenizes a unix command string into a vector of `Commands`
/// @param s String to tokenize from the user
/// @param linecmd pointer to a vector of `Command`s that will be filled by the function
void tokenize(string s, vector<struct Command> *linecmd)
{
    
    bool isBackground = false; //semi-global flag that gets copied to each sub-command
    vector<string> subcmds;

    //Find any comments in our cmd and remove them from s.
    size_t comment = s.find('#');
    if (comment != string::npos)
    {
        s.erase(comment, s.size() - comment);
    }

    //Check to see if we're a background job, & will always have a known pos
    size_t found = s.find('&');
    if (found != string::npos)
    {
        isBackground = true;
        s.erase(found, 1);
    }

    //Spit is a command in built-ins, which returns a vector of strings delimited by, in this case `|`
    subcmds = split(s, '|');

    //iterate over each subcmd, and generate a 
    for (int k = 0; k < subcmds.size(); k++)
    {
        /// @note subcmd has each token removed from it as we iterate through
        string subcmd = subcmds[k]; //get the current subcmd from the array
        struct Command cmd; // We'll fill and then push this struct to the lineCmd vector at the end of this for loop
        cmd.isBackground = isBackground; //set the background flag from the semi-global
        for (int i = 0; i < subcmd.length(); i++) //inter through each subcmd by char and check for special chars or whitespace
        {
            if (subcmd[i] == '<') // We're going to read in from a file, so figure out what file and dup2 its FD to our STDIN
            {
                cmd.hasRead = true; //Set Command flag
                subcmd.erase(0, i + 1); //Erase the special char

                // The next token is what we're reading from, so we grab it and store it in cmd
                for (int j = 1; j < subcmd.length(); j++)
                {
                    if (subcmd[j] == ' ' || j == subcmd.length() - 1)
                    {
                        /*This code is used multiple times, which probably means it should be its own function, but oh well
                        We allocate a c string with size j + 1, j will be the len of our string, we add one for the null term
                        We copy our new token as defined by 'j' */
                        char *temp = (char *)calloc(j+1, sizeof(char));
                        subcmd.copy(temp, j, 1);
                        cmd.readFrom = temp;
                        free(temp);
                        subcmd.erase(0, j + 1);
                        i = -1;
                        continue;
                    }
                }
            }
            if (subcmd[i] == '>' && k == subcmds.size() - 1)
            {
                if (subcmd[i + 1] == '>')
                {
                    cmd.redirectAppend = true;
                    cmd.redirectTo = trim(split(subcmd, '>')[2], ' ');
                    break;
                }
                cmd.hasRedirect = true;
                cmd.redirectTo = trim(split(subcmd, '>')[1], ' ');
                break;
            }
            if (subcmd[i] == '\'' || subcmd[i] == '\"')
            {
                for (int j = (i + 1); j < subcmd.length(); j++)
                {
                    if (subcmd[j] == '\'' || subcmd[j] == '\"')
                    {
                        char *temp = (char *)calloc(j+1, sizeof(char));
                        subcmd.copy(temp, j - 1, 1);
                        cmd.args.push_back(temp);
                        delete temp;
                        subcmd.erase(0, j + 1);
                        i = -1;
                        break;
                    }
                }
                continue;
            }
            // TODO FIX SINGLE COMMANDS
            if (subcmd[i] == ' ' || i == subcmd.length() - 1)
            {
                // The bounds of copy and erase are designed to leave out the trailing white space
                // However if the last character is not a space, we need to include it or the cmd gets cut off
                // This is why lastCharOffset was brought into exististence
                // There is a better way to do this, but messing with the parser at this point is too involved.
                int lastCharOffset = ((i == subcmd.length() - 1) && (subcmd[i] != ' '));
                char *temp = (char *)calloc(i + 1 + lastCharOffset, sizeof(char));
                subcmd.copy(temp, i + lastCharOffset, 0);
                temp[i + lastCharOffset] = '\0'; // Ensure null termination
                subcmd.erase(0, i + 1);
                if (i > 0) // If i == 0, then the command is empty, so don't push it 
                {
                    string tempStr(temp); // is this bad form? I don't know
                    if (tempStr.find('$') != string::npos)
                    {
                        cmd.args.push_back(expandEnvironmentVariables(tempStr));
                    }
                    else
                    {
                        cmd.args.push_back(tempStr); // Push the string, not the char*
                    }
                }
                free(temp); // Use free instead of delete for memory allocated with calloc
                i = -1;
            }
        }
        linecmd->push_back(cmd);
    }
}

string expandEnvironmentVariables(const string &input)
{
    string result = input;
    size_t sPos = 0;

    while ((sPos = result.find('$', sPos)) != string::npos)
    {
        size_t ePos = sPos + 1;
        while (ePos < result.size() && isupper(result[ePos]))
        {
            ++ePos;
        }
        size_t len = ePos - sPos;
        string envName = result.substr(sPos + 1, len - 1);
        const char *envVal = getenv(envName.c_str());

        if (envVal != nullptr)
        {
            result.replace(sPos, len, envVal);
            sPos += strlen(envVal);
        }
        else
        {
            sPos = ePos;
        }
    }

    return result;
}
