/*
	Instructions: Compile using:
			g++ -std=c++11 Assignment1.cpp

	Input to the program is in the csv adjacency
	list format as described in
	https://gephi.org/users/supported-graph-formats/csv-format/
																*/

#include <bits/stdc++.h>

using namespace std;

vector<vector<int> > adj; //Adjacency matrix for tree
map<string, int> vtx2num;
map<int, string> num2vtx;
int ctr = 0;

void csv2adj()
{
	string s, vtx = "";
	while(getline(cin, s))
	{
		s+=';';
		int base = -1;
		for(auto ch: s)
		{
			if((ch==',' || ch==';' || ch=='|' || ch==' ' || ch=='\t') && vtx!="")
			{
				if(base==-1)
				{
					if(vtx2num.find(vtx)==vtx2num.end())
					{
						vtx2num[vtx] = ctr;
						num2vtx[ctr++]=vtx;
						adj.push_back(vector<int>());
					}
					base = vtx2num[vtx];
				}
				else
				{
					if(vtx2num.find(vtx)==vtx2num.end())
					{
						vtx2num[vtx] = ctr;
						num2vtx[ctr++]=vtx;
						adj.push_back(vector<int>());
					}
					adj[base].push_back(vtx2num[vtx]);
					adj[vtx2num[vtx]].push_back(base);
				}
				vtx = "";
			}
			else
			{
				vtx+=ch;
			}
		}
	}
}


//Longest Path

vector<int> dp_lp;
vector<pair<int, int> > ext_nodes;

int getExtremeNodes(int u, int par = -1)
{
	int max1 = 0, max2 = 0, u1 = u, u2 = u;
	for(auto v: adj[u])
	{
		if(v==par) continue;
		int temp = getExtremeNodes(v, u);
		if(temp>max1)
		{
			max2 = max1;
			u2 = u1;
			max1 = temp;
			u1 = ext_nodes[v].first;
		}
		else if(temp>max2)
		{
			max2 = temp;
			u2 = ext_nodes[v].first;
		}
	}
	dp_lp[u] = 1+max1+max2;
	ext_nodes[u] = make_pair(u1, u2);
	return 1+max1;
}

bool printPath(int u, int v, int par=-1)
{
	if(u==v)
	{
		cout<<num2vtx[u];
		return 1;
	}
	for(auto w: adj[u])
	{
		if(w==par) continue;
		if(printPath(w, v, u))
		{
			cout<<", "<<num2vtx[u];
			return 1;
		}
	}
	return 0;
}


//Minimum Vertex Cover

vector<vector<int> > dp_vc;
vector<bool> isSelected, inVertexCover;

void getDPValues_vc(int u, int par=-1)
{
	dp_vc[u][0] = 0;
	dp_vc[u][1] = 1;
	for(auto v: adj[u])
	{
		if(v==par) continue;
		getDPValues_vc(v, u);
		dp_vc[u][0] += dp_vc[v][1];
		if(dp_vc[v][1] < dp_vc[v][0])
		{
			isSelected[v] = true;
		}
		dp_vc[u][1] += min(dp_vc[v][0], dp_vc[v][1]);
	}
}

void constructMinVertexCover(int u, int par=-1)
{
	if(par==-1)
	{
		if(dp_vc[u][0]<dp_vc[u][1])
		{
			inVertexCover[u] = false;
		}
		else inVertexCover[u] = true;
	}
	if(inVertexCover[u]==0)
	{
		inVertexCover[u] = isSelected[u];
		if(isSelected[u]==0)
		{
			for(auto v: adj[u])
			{
				if(v!=par) inVertexCover[v] = true;
			}
		}
	}
	for(auto v: adj[u])
	{
		if(v!=par) constructMinVertexCover(v, u);
	}
}

//Minimum Dominating Set

vector<vector<int> > dp_ds, isSelected_ds;
vector<int> inDominatingSet;

void getDPValues_ds(int u, int par=-1)
{
	dp_ds[u][0] = 0;
	dp_ds[u][1] = 1;
	dp_ds[u][2] = 0;
	int min_diff = 0;
	int min_diff_vertex = -1;
	for(auto v: adj[u])
	{
		if(v==par) continue;
		getDPValues_ds(v, u);
		if(dp_ds[v][0] < dp_ds[v][1])
		{
			dp_ds[u][0] += dp_ds[v][0];
			if(min_diff_vertex==-1 || min_diff>(dp_ds[v][1]-dp_ds[v][0]))
			{
				min_diff = dp_ds[v][1] - dp_ds[v][0];
				min_diff_vertex = v;
			}
		}
		else
		{
			dp_ds[u][0] += dp_ds[v][1];
			isSelected_ds[v][0] = 1;
			if(min_diff_vertex==-1 || min_diff>0)
			{
				min_diff = 0;
				min_diff_vertex = v;
			}
		}
		dp_ds[u][1] += min(dp_ds[v][0], min(dp_ds[v][1], dp_ds[v][2]));
		if(min(dp_ds[v][0], min(dp_ds[v][1], dp_ds[v][2]))==dp_ds[v][0])
		{
			isSelected_ds[v][1] = 0;
		}
		else if(min(dp_ds[v][0], min(dp_ds[v][1], dp_ds[v][2]))==dp_ds[v][1])
		{
			isSelected_ds[v][1] = 1;
		}
		else
		{
			isSelected_ds[v][1] = 2;
		}
		dp_ds[u][2] += dp_ds[v][0];
	}
	if(min_diff_vertex==-1)
	{
		dp_ds[u][0] = 1000000;
	}
	else
	{
		dp_ds[u][0] += min_diff;
		isSelected_ds[min_diff_vertex][0] = 1;
	}
}

void constructMinDominatingSet(int u, int par=-1)
{
	if(par==-1)
	{
		if(dp_ds[u][0]<dp_ds[u][1])
		{
			inDominatingSet[u] = 0;
		}
		else inDominatingSet[u] = 1;
	}
	else if(inDominatingSet[par] == 2)
	{
		inDominatingSet[u] = 0;
	}
	else
	{
		inDominatingSet[u] = isSelected_ds[u][inDominatingSet[par]];
	}
	for(auto v: adj[u])
	{
		if(v!=par) constructMinDominatingSet(v, u);
	}
}

//Maximum Independent Set

vector<vector<int> > dp_is;
vector<bool> inIndependentSet;

void getDPValues_is(int u, int par=-1)
{
	dp_is[u][0] = 0;
	dp_is[u][1] = 1;
	for(auto v: adj[u])
	{
		if(v==par) continue;
		getDPValues_is(v, u);
		dp_is[u][1] += dp_is[v][0];
		if(dp_is[v][0] < dp_is[v][1])
		{
			isSelected[v] = true;
		}
		dp_is[u][0] += max(dp_is[v][0], dp_is[v][1]);
	}
}

void constructMaxIndependentSet(int u, int par=-1)
{
	if(par==-1)
	{
		if(dp_is[u][0]>dp_is[u][1])
		{
			inIndependentSet[u] = false;
		}
		else inIndependentSet[u] = true;
	}
	if(inIndependentSet[u])
	{
		inIndependentSet[u] = isSelected[u];
		if(isSelected[u])
		{
			for(auto v: adj[u])
			{
				if(v!=par) inIndependentSet[v] = false;
			}
		}
	}
	for(auto v: adj[u])
	{
		if(v!=par) constructMaxIndependentSet(v, u);
	}
}

int main()
{
	csv2adj();
	int n = ctr;


	//Longest Path
	dp_lp.resize(n);
	ext_nodes.resize(n);
	getExtremeNodes(0);
	int mx = 0, v1, v2;
	for(int i=0; i<n; i++)
	{
		if(dp_lp[i]>mx)
		{
			v1 = ext_nodes[i].first;
			v2 = ext_nodes[i].second;
			mx = dp_lp[i];
		}
	}
	cout<<"The length of the longest path is "<<mx<<" and the path is:\n";
	printPath(v1, v2);
	cout<<"\n\n";


	//Minimum Vertex Cover
	dp_vc.resize(n);
	for(int i=0; i<n; i++)
	{
		dp_vc[i].resize(2, 0);
	}
	isSelected.resize(n);
	inVertexCover.resize(n);
	getDPValues_vc(0);
	constructMinVertexCover(0);
	int minvc_count = 0;
	for(int i=0; i<n; i++)
	{
		if(inVertexCover[i]) minvc_count++;
	}
	cout<<"The size of the minimum vertex cover is "<<minvc_count<<" and it contains:\n";
	for(int i=0; i<n; i++)
	{
		if(inVertexCover[i])
		{
			if(minvc_count==-1) cout<<", ";
			cout<<num2vtx[i];
			minvc_count = -1;
		}
	}
	cout<<"\n\n";

	//Minimum Dominating Set

	dp_ds.resize(n);
	for(int i=0; i<n; i++)
	{
		dp_ds[i].resize(3, 0);
	}
	isSelected_ds.resize(n);
	for(int i=0; i<n; i++)
	{
		isSelected_ds[i].resize(2, 0);
	}
	inDominatingSet.resize(n, 0);
	getDPValues_ds(0);
	constructMinDominatingSet(0);
	int minds_count = 0;
	for(int i=0; i<n; i++)
	{
		if(inDominatingSet[i]%2) minds_count++;
	}
	cout<<"The size of the minimum dominating set is "<<minds_count<<" and it contains:\n";
	for(int i=0; i<n; i++)
	{
		if(inDominatingSet[i]%2)
		{
			if(minds_count==-1) cout<<", ";
			cout<<num2vtx[i];
			minds_count = -1;
		}
	}
	cout<<"\n\n";

	//Maximum Independent Set
	dp_is.resize(n);
	for(int i=0; i<n; i++)
	{
		dp_is[i].resize(2, 0);
	}
	fill(isSelected.begin(), isSelected.end(), false);
	inIndependentSet.resize(n, true);
	getDPValues_is(0);
	constructMaxIndependentSet(0);
	int maxis_count = 0;
	for(int i=0; i<n; i++)
	{
		if(inIndependentSet[i]) maxis_count++;
	}
	cout<<"The size of the maximum independent set is "<<maxis_count<<" and it contains:\n";
	for(int i=0; i<n; i++)
	{
		if(inIndependentSet[i])
		{
			if(maxis_count==-1) cout<<", ";
			cout<<num2vtx[i];
			maxis_count = -1;
		}
	}
	cout<<"\n\n";

	return 0;
}