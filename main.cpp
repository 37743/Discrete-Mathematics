#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <queue>

using namespace std;

vector<string> split(string str, char delim=',') // Default delimiter is ','.
{   
    vector<string> parts;
    stringstream sstr(str); // instance of stream class that operates on strings
    string part;
    while(getline(sstr,part,delim))
    {
        parts.push_back(part); // adds string before and after given delimiter.
    }
    // for (int i = 0; i<parts.size(); i++) cout<<parts[i]<<"\\";
    return parts;
}

vector<string> parse(string s, vector<string> &infer, string delimiter, int &count) // split but with a string instead of char
{
    int pos_start = 0, pos_end, delim_len = delimiter.length();
    string part;
    vector<string> parts;
    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        part = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        parts.push_back(part);
        if (delimiter == "If ") infer.push_back(" > ");
        else if(delimiter == " and ") infer.push_back("^");
        else if(delimiter == " or ") infer.push_back("v");
        else if(delimiter == " not ") infer.push_back("~");
        else if(delimiter == "Therefore ") infer.push_back("------\n");
        else if(delimiter == "Tuesday") infer.push_back("p");
        else if(delimiter == "test in English") infer.push_back("q");
        else if(delimiter == "Science") infer.push_back("r");
        else if(delimiter == "English Professor") infer.push_back("k");
        else if(delimiter == "\n")
        {
            if (infer.back() != " > ")
            {
                infer.push_back("\n"); // indicates the end of line, skip if implication is still due.
                count++;
            }
        }
    }
    parts.push_back(s.substr(pos_start));
    return parts;
}

void buildStatement(queue<string> &statements, string statement,string &premise1, string &premise2, string &oper)
{
    int operpos = statement.find(oper);
    premise1 = statement.substr(0,operpos);
    oper = statement.substr(operpos,operpos);
    premise2 = statement.substr(operpos+1);
    // erase spaces
    premise1.erase(remove(premise1.begin(), premise1.end(), ' '), premise1.end());
    premise2.erase(remove(premise2.begin(), premise2.end(), ' '), premise2.end());
    statements.push(premise1);
    cout<<statements.back()<<endl;
    statements.push(oper);
    cout<<statements.back()<<endl;
    statements.push(premise2);
    cout<<statements.back()<<endl;
}

bool modusPonens(queue<string> &statements, string premise1, string premise2)
{
    if (premise2.substr(0,premise2.find(">"))==premise1) {statements.push(premise2.substr(premise2.find(">")+1)); return true;}
    else if(premise1.substr(0,premise1.find(">"))==premise2) {statements.push(premise1.substr(premise1.find(">")+1)); return true;}
    else{statements.push("NULL"); return false;}
}

bool modusTollens(queue<string> &statements, string premise1, string premise2)
{
    if ("~"+premise2.substr(premise2.find(">")+1)==(premise1)) {statements.push("~"+premise2.substr(0,premise2.find(">"))); return true;}
    else if("~"+premise1.substr(premise1.find(">")+1)==(premise2)) {statements.push("~"+premise1.substr(premise1.find(">")+1)); return true;}
    else{statements.push("NULL"); return false;}
}

bool hypoSyllo(queue<string> &statements, string premise1, string premise2)
{
    if (premise1.substr(premise1.find(">")+1)==premise2.substr(0,premise2.find(">"))) 
    {
        statements.push(premise1.substr(0,premise1.find(">"))+premise2.substr(premise2.find(">")));
        return true;
    }
    else if (premise2.substr(premise2.find(">")+1)==premise1.substr(0,premise1.find(">"))) 
    {
        statements.push(premise2.substr(0,premise2.find(">"))+premise1.substr(premise1.find(">")));
        return true;
    }
    else{statements.push("NULL"); return false;}
}

bool disjuncSyllo(queue<string> &statements, string premise1, string premise2)
{
    if ("~"+premise1.substr(0,premise1.find("v"))==premise2) 
    {
        statements.push(premise1.substr(premise1.find("v")+1));
        return true;
    }
    else if ("~"+premise2.substr(0,premise2.find("v"))==premise1) 
    {
        statements.push(premise2.substr(premise2.find("v")+1));
        return true;
    }
    else{statements.push("NULL"); return false;}
}

bool simplification(queue<string> &statements, string premise1, string premise2)
{
    if ((premise1.find("^")<premise1.length()) && (premise2=="")) 
    {
        statements.push(premise1.substr(0,premise1.find("^")));
        statements.push(premise1.substr(premise1.find("^")+1));
        return true;
    }
    else if ((premise2.find("^")<premise2.length()) && (premise1=="")) 
    {
        statements.push(premise2.substr(0,premise2.find("^")));
        statements.push(premise2.substr(premise2.find("^")+1));
        return true;
    }
    else{statements.push("NULL"); return false;}
}

bool conjunction(queue<string> &statements, string premise1, string premise2)
{
    if ((premise1!="") && (premise2!="")) 
    {
        statements.push(premise1);
        statements.push(premise2);
        return true;
    }
    else if ((premise1!="")&&(premise2==""))
    {
        statements.push(premise1);
        return true;
    }
    else if ((premise1=="")&&(premise2!=""))
    {
        statements.push(premise2);
        return true;
    }
    else{statements.push("NULL"); return false;}
}

bool resolution(queue<string> &statements, string premise1, string premise2)
{
    if ("~"+premise1.substr(0,premise1.find("v"))==premise2.substr(0,premise2.find("v"))) 
    {
        statements.push(premise1.substr(premise1.find("v")+1)+"v"+premise2.substr(premise2.find("v")+1));
        return true;
    }
    else if ("~"+premise2.substr(0,premise2.find("v"))==premise1.substr(0,premise1.find("v"))) 
    {
        statements.push(premise2.substr(premise2.find("v")+1)+"v"+premise1.substr(premise1.find("v")+1));
        return true;
    }
    else{statements.push("NULL"); return false;}
}

int main()
{
    vector<vector <string>> rulesFile;
    ifstream File1("./Rules-of-Inference/InferenceRules.csv");
	if (File1.is_open())
    {
        int i=1;
        bool skippedFirst = false;
        string str;
        while(getline(File1,str)) // Select every line and store in str of type String.
        {
            if (!skippedFirst)
            {
                skippedFirst = true;
                cout<<"Inference Rules File Contents:\n---------------------------\n"
                    <<"Prem.1\t"<<"|Prem.2\t"<<"|Conc.\t"<<"|Rule\t"<<endl;
                continue;
            } // Skips the first line with the "Premise1,Premise2,Conclusion,Rule"
            vector<string> parts = split(str,',');
            rulesFile.push_back({parts[0],parts[1],parts[2],parts[3]}); // 0 is Premise1, 1 is Premise2,
                                                                        // 2 is Conclusion, and 3 is Rule.
            cout<<rulesFile[i-1][0]<<"\t|"<<rulesFile[i-1][1]<<"\t|"<<rulesFile[i-1][2]<<"\t|"<<rulesFile[i-1][3]<<endl;                                                       
            i++;
        }
    }
	File1.close();

    vector<string> englishFile;
    ifstream File2("./Project/test.txt");
	if (File2.is_open())
    {
        int i=0;
        string str;
        cout<<"\nArgument:"
        <<"\n---------------------------\n";
        while(getline(File2,str)) // Select every line and store in str of type String.
        {
            string const delims{".,:;!?"};
            int pos_start, pos_end = 0;
            while ((pos_start = str.find_first_not_of(delims, pos_end)) != string::npos)
            {
                pos_end = str.find_first_of(delims, pos_start + 1);
                englishFile.push_back(str.substr(pos_start, pos_end - pos_start)+"\n");
                cout<<englishFile[i];
                i++;
            }
        }
    }
	File2.close();

    // Building the argument.
    vector<string> inferFile;
    int count=0;
    cout<<"\nEnglish to Inference Rules:"
    <<"\n---------------------------\n";
    for (auto str: englishFile)
    {
        vector<string> parsing = {
        " not ",
        "Tuesday",
        " and ",
        "English Professor",
        "Therefore ",
        "test in English",
        "If ",
        " or ",
        "Science",
        "\n"
        };
        // Parse into inference rules.
        for (auto i: parsing) parse(str,inferFile,i,count);
    }
    for (auto v: inferFile) cout<<v;

    // Decision Making.
    cout<<"\nDecision Making:"
    <<"\n---------------------------\n";
    // Merge each expression to a string.
    vector<string> newInfer;
    int i=0;
    for (int j=0; j<count; j++)
    {
        string newstr = inferFile[i];
        while (inferFile[i+1]!="\n")
        {
            if (inferFile[i+1]=="------\n") i++;
            newstr += inferFile[i+1];
            i++;
        }
        i++;
        newInfer.push_back(newstr.substr(newstr.find("\n")+1));
    }
    // for (auto i: newInfer) cout<<i<<endl;

    // queue<string> statementstest;
    // for (auto i: newInfer)
    // {
    //     string statement = i;
    //     string premise1 = "temp";
    //     string premise2 = "temp";

    //     vector<string> opervec = {">","^"};
    //     for (auto oper: opervec)
    //     {
    //         if (statement.find(oper)<statement.length())
    //         {
    //             buildStatement(statementstest, statement, premise1, premise2, oper);
    //             continue;
    //         }
    //     }
    // }
    // cout<<"\n------\n";

    // Computation (Manual)
    // Step 1
    queue<string> statements;
    string prem1="";
    string prem2="";
    prem1 = newInfer[2];
    prem1.erase(remove(prem1.begin(), prem1.end(), ' '), prem1.end());
    string res = simplification(statements,prem1,prem2)? "Success!": "Fail!";
    cout<<"Step 1:\nSimplification: "<<"\n"+prem1+"\n" + prem2 + "\n"<<res<<"\nResult: "+statements.front()+" "+statements.back()<<endl;
    // Step 2
    prem1 = newInfer[0];
    prem1.erase(remove(prem1.begin(), prem1.end(), ' '), prem1.end());
    prem2 = statements.front();
    statements.pop();
    res = modusPonens(statements,prem1,prem2) ? "Success!": "Fail!";
    cout<<"------\n"
    <<"Step 2:\nModus Ponens: "<<"\n"+prem1+"\n" + prem2 + "\n"<<res<<"\nResult: "+statements.back()<<endl;
    // Step 3
    prem1 = newInfer[1];
    prem1.erase(remove(prem1.begin(), prem1.end(), ' '), prem1.end());
    prem2=statements.front();
    statements.pop();
    res = modusPonens(statements,prem1,prem2) ? "Success!": "Fail!";
    cout<<"------\n"
    <<"Step 3:\nModus Ponens: "<<"\n"+prem1+"\n" + prem2 + "\n"<<res<<"\nResult: "+statements.back()<<endl;
    // Step 4
    prem1 = statements.front();
    statements.pop();
    prem2 = statements.front();
    statements.pop();
    res = disjuncSyllo(statements,prem1,prem2) ? "Success!": "Fail!";
    cout<<"------\n"
    <<"Step 4:\nDisjunctive Syllogism: "<<"\n"+prem1+"\n" + prem2 + "\n"<<res<<"\nResult: "+statements.back()<<endl;
    // Step 5
    res = statements.front()==newInfer.back() ? "valid!":"invalid!";
    cout<<"------\n"
    <<"Step 5:\n"<<statements.front()<<" == "<<newInfer.back()<<endl
    <<"The argument is "<<res<<endl;
}