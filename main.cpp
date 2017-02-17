#include <iostream>
#include <fstream>

using namespace std;

bool isStr(string s);
int convertToNum(string s);
int numOfRouter=-1;

//int graph[50][50][50];


void Dijkstra(int index, int v,int target);


class LSP
{
public:
    int id;
    int pre;
    int sequenceNum;
    int timeToLive;
    int li[50];
    int cost[50];

    LSP()
    {
        id=-1;
        pre=-1;
        sequenceNum=1;
        timeToLive=10;
        for (int i=0;i<50;i++)
        {
            li[i]=-1;
            cost[i]=1;
        }
    }
};


class Router
{
public:
    int id;
    int connect[50];
    Router *refer[50];
    int cost[50];
    int numofconnect;
    string network;
    int sequence;
    int history1[50];
    int history2[50];
    int netcost;
    //int tick[50];
    int graph[50][50];
    int numOfHistory;
    LSP packetToSend[1000];
    int numOfPacket;
    bool on;
    int tick[50];


    Router()
    {
        id=0;
        numofconnect=0;
        on=true;
        numOfPacket=0;
        for (int i=0;i<50;i++)
        {
            tick[i]=0;
            connect[i]=0;
            cost[i]=1;
            history1[i]=-1;
            history2[i]=0;
            refer[i]=NULL;
            for (int j=0;j<50;j++)
                graph[i][j]=999999;
        }
       // for (int i=0;i<1000;i++)
       //     packetToSend[i]=new LSP();
        network="";
        sequence=0;
        numOfHistory=0;
        netcost=1;
    }




    bool receivePacket(LSP packet)
    {
        if (on==false)
            return false;
        packet.timeToLive--;
        if (packet.timeToLive==0)
            return false;

        for (int i=0;i<numOfHistory;i++)
        {
            if (history1[i]==packet.id && history2[i]>=packet.sequenceNum)
                return false;
        }
        history1[numOfHistory]=packet.id;
        history2[numOfHistory]=packet.sequenceNum;
        numOfHistory++;


        packetToSend[numOfPacket]=packet;

        numOfPacket++;

        return true;

    }


    void originatePacket()
    {
        if (on==false)
            return;
        LSP packet;
        packet.id=id;
        sequence++;
        packet.sequenceNum=sequence;
        for (int i=0;i<numofconnect;i++)
        {
            packet.li[i]=connect[i];
            packet.cost[i]=cost[i];
        }

        packet.pre=id;
        packetToSend[numOfPacket]=packet;
        numOfPacket++;
    }

};

Router router[50];
void setGraph();
void print(int i);
void update(int target,LSP p);

int main()
{
    ifstream infile;
    infile.open("infile.dat");
    if (!infile)
    {
        cout << "The store data is not existing!";
        return 0;
    }

    char c;
    string s[3000];


    for (int i=0;i<3000;i++)
    {
        s[i]="";
    }
    int k=0;

    infile>>noskipws;

    while (infile>>c)
    {
        if (c!='\t' && c!=' ' && c!='\n')
            s[k]+=c;
        else if (c=='\t' || c==' ')
        {
            k++;
            s[k]=" ";
            k++;
        }

        else if (c=='\n')
        {
            k++;
            s[k]="newline";
            k++;
        }
    }

    infile.close();

    k++;
    s[k]="newline";



    for (int i=0;i<k;i++)
    {
        string temp[3];
        int j=0;
        while (s[i]!="newline")
        {
            if (s[i]!=" " && s[i]!="")
            {
                temp[j]=s[i];
                j++;
            }
            i++;
        }

        if (j==3)
        {
            numOfRouter++;
            router[numOfRouter].id=convertToNum(temp[0]);
            router[numOfRouter].network=temp[1];
            router[numOfRouter].netcost=convertToNum(temp[2]);
        }
        else if (j==1)
        {
            router[numOfRouter].connect[router[numOfRouter].numofconnect]=convertToNum(temp[0]);
            router[numOfRouter].numofconnect++;
        }
        else if (j==2)
        {
            if (isStr(temp[1]))
            {
                numOfRouter++;
                router[numOfRouter].id=convertToNum(temp[0]);
                router[numOfRouter].network=temp[1];
            }
            else
            {
                router[numOfRouter].connect[router[numOfRouter].numofconnect]=convertToNum(temp[0]);
                router[numOfRouter].cost[router[numOfRouter].numofconnect]=convertToNum(temp[1]);
                router[numOfRouter].numofconnect++;
            }
        }
    }

    numOfRouter++;
    setGraph();


    for (int i=0;i<numOfRouter;i++)
    {
        for (int j=0;j<router[i].numofconnect;j++)
        {
            for (k=0;k<numOfRouter;k++)
            {
                if (router[k].id==router[i].connect[j])
                    router[i].refer[j]=&router[k];
            }
        }
    }



    cout<<"Would you like to continue (enter C), quit (enter Q), print the routing table of a router (P followed by the router's id number), shut down a router (enter S followed by the id number), or start up a router (enter T followed by the id)?"<<endl;

    char ch;

    while(cin>>ch)
    {
        if (ch=='C')
        {
            for (int i=0;i<numOfRouter;i++)
            {
                router[i].originatePacket();
                for (int j=0;j<router[i].numOfPacket;j++)
                {
                    LSP p=router[i].packetToSend[j];
                    for (int k=0;k<router[i].numofconnect;k++)
                    {
                        int target;
                        for (int l=0;l<numOfRouter;l++)
                        {
                            if (router[i].connect[k]==router[l].id)
                            {
                                target=l;
                            }
                        }
                        if (router[i].connect[k]!=p.pre)
                        {
                            p.pre=router[i].id;
                            if (router[target].on==true)
                            {
                                if (router[target].receivePacket(p))
                                {
                                    update(target,p);
                                    continue;
                                }
                            }
                        }
                        for (int l=0;l<router[target].numofconnect;l++)
                        {
                            if (router[target].connect[l]==router[i].id)
                            {
                                router[target].tick[l]++;
                                if (router[target].tick[l]==2)
                                {
                                    router[target].graph[target][i]=999999;
                                    router[target].graph[i][target]=999999;
                                }
                            }
                        }
                    }


                }
                router[i].numOfPacket=0;
            }

        }
        else if (ch=='Q')
            return 0;
        else if (ch=='P')
        {
            int num;
            cin>>num;
            for (int i=0;i<numOfRouter;i++)
            {
                if (router[i].id==num)
                    print(i);
            }
        }
        else if (ch=='S')
        {
            int num;
            cin>>num;
            for (int i=0;i<numOfRouter;i++)
            {
                if (router[i].id==num)
                    router[i].on=false;
            }
        }
        else if (ch=='T')
        {
            int num;
            cin>>num;
            for (int i=0;i<numOfRouter;i++)
            {
                if (router[i].id==num)
                    router[i].on=true;;
            }
        }
        cout<<"Would you like to continue (enter C), quit (enter Q), print the routing table of a router (P followed by the router's id number), shut down a router (enter S followed by the id number), or start up a router (enter T followed by the id)?"<<endl;

    }



}


bool isStr(string s)
{
    int l=s.length();
    for (int j=0;j<l;j++)
    {
        if (s[j]=='.')
        {
            return true;
        }
    }
    return false;
}

int convertToNum(string s)
{
    int l=s.length();
    int sum=0;
    for (int i=l-1;i>=0;i--)
    {
        int digit=s[i]-'0';
        for (int j=0;j<l-i-1;j++)
        {
            digit*=10;
        }
        sum+=digit;
    }
    return sum;
}

void setGraph()
{

        for (int i=0;i<numOfRouter;i++)
        {
            for (int j=0;j<router[i].numofconnect;j++)
            {
                for (int k=0;k<numOfRouter;k++)
                {
                    if (router[k].id==router[i].connect[j])
                    {
                        router[i].graph[i][k]=router[i].cost[j];
                        router[i].graph[k][i]=router[i].cost[j];
                    }
                }

            }

        }

   // cout<<router[1].numofconnect;
}

void update(int target, LSP packet)
{
    int source=0;
    for (int i=0;i<numOfRouter;i++)
    {
        if (packet.id==router[i].id)
        {
            source=i;
        }
    }
    for (int i=0;i<numOfRouter;i++)
    {
        for (int j=0;j<50;j++)
        {
            if (packet.li[j]==router[i].id && i!=source)
            {
                router[target].graph[i][source]=packet.cost[j];
                router[target].graph[source][i]=packet.cost[j];
                router[target].graph[i][i]=0;
            }
        }

    }

}


void Dijkstra(int v,int target)
{
	bool s[50];
	int dist[50];
	int prev[50];
	for(int i=0; i<numOfRouter; ++i)
	{
		dist[i] = router[v].graph[v][i];
		s[i] = false;

        if(dist[i] == 999999)
			prev[i] = -1;
		else
			prev[i] = v;

	}
	dist[v] = 0;
	s[v] = true;


	for(int i=0; i<numOfRouter; ++i)
	{
	    if (i!=v)
        {
            int temp = 999999;
            int u = v;
            for(int j=0; j<numOfRouter; ++j)
                if((!s[j]) && dist[j]<temp)
                {
                    u = j;
                    temp = dist[j];
                }
            s[u] = true;
            if (u==target)
            {
                while (prev[u]!=v)
                {
                    u=prev[u];
                }
                cout<<dist[target]<<", "<<u<<endl;
                return;
            }
            for(int j=0; j<numOfRouter; ++j)
            {
                if((!s[j]) && router[v].graph[u][j]<999999)
                {
                    int newdist = dist[u] + router[v].graph[u][j];
                    if(newdist < dist[j])
                    {
                        dist[j] = newdist;
                        prev[j] = u;
                    }
                }
            }
        }

	}
}

void print(int source)
{
    for (int i=0;i<numOfRouter;i++)
    {
        for (int j=0;j<numOfRouter;j++)
        {
            if (i!=source && router[source].graph[i][j]!=999999)
            {
                cout<<router[i].network<<", ";
                Dijkstra(source,i);
                break;
            }
        }

    }
}

