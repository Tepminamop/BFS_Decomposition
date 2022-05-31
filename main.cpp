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
using std::pair;
using std::fstream;
using std::ios;
#define CORES 4

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

void print_answer_to_file(const vector<pair<int, int>>& TYPE_ID_SET, const int& n) {
	fstream answer_output_file;
	answer_output_file.open("decomposition.txt", ios::out);
	for (int i = 0; i < n; i++) {
		answer_output_file << i << " " << TYPE_ID_SET[i].first << " " << TYPE_ID_SET[i].second << '\n';
	}

}

void split_parts_into_id_subsets(const vector<pair<int, int>>& parts_of_type_set, const vector<int>& count, vector<int>& count_type_subset, map<int, int>& parts_to_subset) {
	for (int i = 0; i < (int)parts_of_type_set.size(); i++) {
		if (parts_to_subset.find(parts_of_type_set[i].first + 1) != parts_to_subset.end()) {
			//if parts are connected, put them in the same subset
			//this should never be entered
			assert(false);
			count_type_subset[parts_to_subset[parts_of_type_set[i].first + 1]] += count[parts_of_type_set[i].first];
			parts_to_subset[parts_of_type_set[i].first] = parts_to_subset[parts_of_type_set[i].first + 1];
			continue;
		}
		else if (parts_to_subset.find(parts_of_type_set[i].first - 1) != parts_to_subset.end()) {
			//if parts are connected, put them in the same subset
			//this should never be entered
			assert(false);
			count_type_subset[parts_to_subset[parts_of_type_set[i].first - 1]] += count[parts_of_type_set[i].first];
			parts_to_subset[parts_of_type_set[i].first] = parts_to_subset[parts_of_type_set[i].first - 1];
			continue;
		}

		int minimum_subset = 0;
		for (int i = 1; i < 4; i++) {
			if (count_type_subset[i] < count_type_subset[minimum_subset]) {
				minimum_subset = i;
			}
		}

		count_type_subset[minimum_subset] += count[parts_of_type_set[i].first];
		parts_to_subset[parts_of_type_set[i].first] = minimum_subset;
	}
}

void input_graph_from_file(vector<vector<int>>& input_data,
	const int& count_vertices,
	int& input_count_edges, const string& filename) {
	string str;
	fstream in;
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
	else {
		cout << "Error with file!";
		assert(false);
	}
}

int main() {
	string filename;
	cout << "Input filename: " << '\n';
	cin >> filename;
	cout << "Input number of vertices: " << '\n';
	int input_count_vertices;
	cin >> input_count_vertices;

	const int COUNT_VERTICES = input_count_vertices;
	vector<vector<int>> input_data(COUNT_VERTICES);
	int input_count_edges = -1;
	input_graph_from_file(input_data, COUNT_VERTICES, input_count_edges, filename);
	const int COUNT_EDGES = input_count_edges + 1;

	vector<vector<int>> incidence_list(COUNT_EDGES);
	make_incidence_list(incidence_list, input_data);

	vector<vector<int>> adjacent_list(COUNT_VERTICES);
	make_adjacent_list(incidence_list, adjacent_list);

	//use bfs to make graph partition
	int best = INT_MIN;
	for (int i = 0; i < 100; i++) {
		srand(time(NULL));
		int start_pos = rand() % COUNT_VERTICES;
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

		//count vertices in each part
		vector<int> count(maximum + 1, 0);
		for (int i = 0; i < COUNT_VERTICES; i++) {
			count[destination[i]]++;
		}

		//allocate parts in type sets (zero set or one set)
		int cnt0 = 0, cnt1 = 0;
		vector<pair<int, int>> parts_of_type_set_0, parts_of_type_set_1;
		for (int i = 0; i <= maximum; i++) {
			//maybe make another partitioning
			if (i % 2 == 0) {
				cnt0 += count[i];
				parts_of_type_set_0.push_back({ i, count[i] });
			}
			else {
				cnt1 += count[i];
				parts_of_type_set_1.push_back({ i, count[i] });
			}
		}

		sort(parts_of_type_set_0.begin(), parts_of_type_set_0.end(), [](pair<int, int> a, pair<int, int> b) {
			return a.second > b.second;
			});

		sort(parts_of_type_set_1.begin(), parts_of_type_set_1.end(), [](pair<int, int> a, pair<int, int> b) {
			return a.second > b.second;
			});

		//count vertices in each subset in type_set
		vector<int> count_type_subset_0(4, 0);
		vector<int> count_type_subset_1(4, 0);

		//need to know in which type_set every part is located
		map<int, int> parts_to_subset_0;
		map<int, int> parts_to_subset_1;

		//split parts in id_sets in each type set
		split_parts_into_id_subsets(parts_of_type_set_0, count, count_type_subset_0, parts_to_subset_0);
		split_parts_into_id_subsets(parts_of_type_set_1, count, count_type_subset_1, parts_to_subset_1);;

		//TYPE_ID_SET.first = 0 or 1 (time period (set), type_set)
		//TYPE_ID_SET.second -> NO of subset (CORES subsets in each time period(type_set))
		vector<pair<int, int>> TYPE_ID_SET(COUNT_VERTICES, { -1, -1 });
		for (int i = 0; i < COUNT_VERTICES; i++) {
			int time_period;//type_set
			int subset;//id_subset
			int part = destination[i];
			if (parts_to_subset_0.find(part) != parts_to_subset_0.end()) {
				time_period = 0;
				subset = parts_to_subset_0[part];
			}
			else {
				time_period = 1;
				subset = parts_to_subset_1[part];
			}

			TYPE_ID_SET[i].first = time_period;//type_set
			TYPE_ID_SET[i].second = subset;//id_subset
		}

		assert(cnt0 + cnt1 == COUNT_VERTICES);
		assert(count_type_subset_0[0] + count_type_subset_0[1] + count_type_subset_0[2] + count_type_subset_0[3] == cnt0);
		assert(count_type_subset_1[0] + count_type_subset_1[1] + count_type_subset_1[2] + count_type_subset_1[3] == cnt1);

		if ((count_type_subset_0[3] + count_type_subset_1[3]) / 2 > best) {
			best = (count_type_subset_0[3] + count_type_subset_1[3]) / 2;
			print_answer_to_file(TYPE_ID_SET, COUNT_VERTICES);
		}
	}

	return 0;
}
