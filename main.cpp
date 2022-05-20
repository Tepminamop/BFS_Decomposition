#include <iostream>
#include <vector>
#include <ctime>
#include <queue>
#include <map>
#include <algorithm>
#include <time.h>
#include <sstream>
#include <fstream>
#include <string>
#include <cassert>

using std::clock;
using std::vector;
using std::queue;
using std::map;
using std::pair;
using std::cin;
using std::cout;
using std::max;
using std::min;
using std::min_element;
using std::max_element;
using std::string;
using std::stringstream;
#define CORES 4

//1) transform input data into adjacent list (input_data -> incidence_list -> adjacent_list)
//2) use graph partitioning algorithm (fiduccia or bfs (bfs is much more simplier but maybe its wrong))
//3) thats all???

void input(vector<vector<int>>& input_data, const int& count_vertices, int& input_count_edges) {
	for (int i = 0; i < count_vertices; i++) {
		int vertice;
		cin >> vertice;

		while (cin.peek() != '\n') {
			int input;
			cin >> input;
			input_data[vertice].push_back(input);
			input_count_edges = max(input_count_edges, input);
		}
	}
}

void make_incidence_list(vector<vector<int>>& incidence_list, const vector<vector<int>>& input_data) {
	for (int i = 0; i < (int)input_data.size(); i++) {
		for (int j = 0; j < (int)input_data[i].size(); j++) {
			incidence_list[input_data[i][j]].push_back(i);
		}
	}
}

void make_adjacent_list(const vector<vector<int>>& incidence_list, vector<vector<int>>& adjacent_list) {
	for (int i = 0; i < (int)incidence_list.size(); i++) {
		for (int j = 0; j < (int)incidence_list[i].size(); j++) {
			for (int k = 0; k < (int)incidence_list[i].size(); k++) {
				if (k == j) continue;
				adjacent_list[incidence_list[i][j]].push_back(incidence_list[i][k]);
			}
		}
	}
}

void dfs(vector<bool>& used, vector<vector<int>>& graph, int v) {
	used[v] = true;
	for (int i = 0; i < (int)graph[v].size(); i++) {
		if (used[graph[v][i]] == false) {
			dfs(used, graph, graph[v][i]);
		}
	}
}

void bfs(vector<int>& parent, vector<int>& destination, const vector<vector<int>>& graph, vector<bool>& used, const int& n, const int& start, int& maximum, const int& start_destination) {
	queue<int> q;
	destination[start] = start_destination;
	maximum = max(maximum, start_destination);
	used[start] = true;
	parent[start] = -1;
	q.push(start);
	while (!q.empty()) {
		int vertice = q.front();
		q.pop();
		for (int i = 0; i < (int)graph[vertice].size(); i++) {
			int to = graph[vertice][i];
			if (!used[to]) {
				used[to] = true;
				q.push(to);
				destination[to] = destination[vertice] + 1;
				parent[to] = vertice;
				maximum = max(maximum, destination[to]);
			}
		}
	}
}

void print_answer(const vector<pair<int, int>>& TYPE_ID_SET, const int& n) {
	for (int i = 0; i < n; i++) {
		cout << TYPE_ID_SET[i].first << " " << TYPE_ID_SET[i].second << '\n';
	}
}

void split_parts_into_id_subsets(const vector<int>& parts_of_type_set, const vector<int>& count, vector<int>& count_type_subset, map<int, int>& parts_to_subset) {
	for (int i = 0; i < (int)parts_of_type_set.size(); i++) {
		if (parts_to_subset.find(parts_of_type_set[i] + 1) != parts_to_subset.end()) {
			//if parts are connected, put them in the same subset
			count_type_subset[parts_to_subset[parts_of_type_set[i] + 1]] += count[parts_of_type_set[i]];
			parts_to_subset[parts_of_type_set[i]] = parts_to_subset[parts_of_type_set[i] + 1];
			continue;
		}
		else if (parts_to_subset.find(parts_of_type_set[i] - 1) != parts_to_subset.end()) {
			//if parts are connected, put them in the same subset
			count_type_subset[parts_to_subset[parts_of_type_set[i] - 1]] += count[parts_of_type_set[i]];
			parts_to_subset[parts_of_type_set[i]] = parts_to_subset[parts_of_type_set[i] - 1];
			continue;
		}

		int minimum_subset = 0;
		for (int i = 1; i < 4; i++) {
			if (count_type_subset[i] < count_type_subset[minimum_subset]) {
				minimum_subset = i;
			}
		}

		count_type_subset[minimum_subset] += count[parts_of_type_set[i]];
		parts_to_subset[parts_of_type_set[i]] = minimum_subset;
	}
}

void input_graph_from_file(vector<vector<int>>& input_data,
	const int& count_vertices,
	int& input_count_edges, const string& filename) {
	string str;
	std::fstream in;
	in.open(filename);
	if (in) {
		while (!in.eof()) {
			while (getline(in, str, '\n') && !in.eof()) {
				vector<int> result;
				stringstream ss(str);
				string item;
				while (getline(ss, item, ' ')) {
					result.push_back(stoi(item));
				}
				for (int i = 1; i < result.size(); i++) {
					input_data[result[0]].push_back(result[i]);
					input_count_edges = max(input_count_edges, result[i]);
				}
				result.clear();
			}
		}
		in.close();
	}
	else(cout << "Error with file!");
}

int main() {
	cout << "Input number of vertices: " << '\n';
	int input_count_vertices;
	cin >> input_count_vertices;

	const int COUNT_VERTICES = input_count_vertices;
	vector<vector<int>> input_data(COUNT_VERTICES);
	int input_count_edges = -1;
	//input(input_data, COUNT_VERTICES, input_count_edges);
	string filename;
	cout << "Input filename: " << '\n';
	//cin >> filename;
	filename = "TRINITY_1.nls";
	input_graph_from_file(input_data, COUNT_VERTICES, input_count_edges, filename);
	const int COUNT_EDGES = input_count_edges + 1;

	const clock_t adjacent_list_time = clock();

	vector<vector<int>> incidence_list(COUNT_EDGES);
	make_incidence_list(incidence_list, input_data);

	vector<vector<int>> adjacent_list(COUNT_VERTICES);
	make_adjacent_list(incidence_list, adjacent_list);

	cout << "Time to make adjacent list: " << float(clock() - adjacent_list_time) / CLOCKS_PER_SEC << '\n';

	srand(time(NULL));
	int start_pos = rand() % COUNT_VERTICES;

	//use bfs to make graph partition
	const clock_t bfs_time = clock();
	vector<bool> used(COUNT_VERTICES, false);
	int maximum = 0;
	vector<int> destination(COUNT_VERTICES, -1);
	vector<int> parent(COUNT_VERTICES, -1);
	bfs(parent, destination, adjacent_list, used, COUNT_VERTICES, start_pos, maximum, maximum);
	for (int i = 0; i < COUNT_VERTICES; i++) {
		if (used[i] == false) {
			bfs(parent, destination, adjacent_list, used, COUNT_VERTICES, i, maximum, maximum + 2);
		}
	}
	cout << "Parts: " << maximum << '\n';
	cout << "Time to make bfs: " << float(clock() - bfs_time) / CLOCKS_PER_SEC << '\n';

	//count vertices in each part
	vector<int> count(maximum + 1, 0);
	for (int i = 0; i < COUNT_VERTICES; i++) {
		count[destination[i]]++;
	}

	for (int i = 0; i < count.size(); i++) {
		cout << count[i] << " ";
	}
	cout << '\n';

	//allocate parts in type sets (zero set or one set)
	int cnt0 = 0, cnt1 = 0;
	vector<int> parts_of_type_set_0, parts_of_type_set_1;
	for (int i = 0; i <= maximum; i++) {
		if (i % 2 == 0) {
			cnt0 += count[i];
			parts_of_type_set_0.push_back(i);
		}
		else {
			cnt1 += count[i];
			parts_of_type_set_1.push_back(i);
		}
	}
	//count in zero set and in first set
	cout << cnt0 << " " << cnt1 << '\n';

	//split parts in id_sets in each type set
	vector<int> count_type_subset_0(4, 0);
	vector<int> count_type_subset_1(4, 0);

	map<int, int> parts_to_subset_0;
	map<int, int> parts_to_subset_1;

	split_parts_into_id_subsets(parts_of_type_set_0, count, count_type_subset_0, parts_to_subset_0);
	split_parts_into_id_subsets(parts_of_type_set_1, count, count_type_subset_1, parts_to_subset_1);

	cout << "subsets of set 0: ";
	for (int i = 0; i < 4; i++) {
		cout << count_type_subset_0[i] << " ";
	}
	cout << '\n';

	cout << "subsets of set 1: ";
	for (int i = 0; i < 4; i++) {
		cout << count_type_subset_1[i] << " ";
	}
	cout << '\n';

	//TYPE_ID_SET.first = 0 or 1 (time period (set))
	//TYPE_ID_SET.second -> NO of subset (CORES subsets in each time period)
	vector<pair<int, int>> TYPE_ID_SET(COUNT_VERTICES, { -1, -1 });
	for (int i = 0; i < COUNT_VERTICES; i++) {
		int time_period;
		int subset;
		int part = destination[i];
		if (parts_to_subset_0.find(part) != parts_to_subset_0.end()) {
			time_period = 0;
			subset = parts_to_subset_0[part];
		}
		else {
			time_period = 1;
			subset = parts_to_subset_1[part];
		}

		TYPE_ID_SET[i].first = time_period;
		TYPE_ID_SET[i].second = subset;
	}

	assert(cnt0 + cnt1 == COUNT_VERTICES);
	assert(count_type_subset_0[0] + count_type_subset_0[1] + count_type_subset_0[2] + count_type_subset_0[3] == cnt0);
	assert(count_type_subset_1[0] + count_type_subset_1[1] + count_type_subset_1[2] + count_type_subset_1[3] == cnt1);

	//print_answer(TYPE_ID_SET, COUNT_VERTICES);

	return 0;
}
