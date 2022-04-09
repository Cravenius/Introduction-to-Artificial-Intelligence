#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<fstream>
#include<unordered_map>
#include<set>
#include<random>

using namespace std;

struct WorldData {
	vector<pair<double, double>> city_coords;
	vector<vector<int>> dist; // adjency matrix with euclodian distance
	vector<vector<double>> attractiveness;
	vector<vector<double>> time;
	int N;

	int EuclideanDistance(int cx, int cy, int gx, int gy) {
		return sqrt(pow((cx - gx), 2) + pow((cy - gy), 2));
	}

	WorldData() {
		ifstream ifs("TSP_City.txt");

		if (ifs.is_open()) {
			int x, y;
			ifs >> N;

			for (int i = 0; i < N; ++i) {
				ifs >> x >> y;
				city_coords.push_back(make_pair(x, y));
			}

			dist.resize(N, vector<int>(N, 0));
			attractiveness.resize(N, vector<double>(N));
			time.resize(N, vector<double>(N, 1.0));

			for (int i = 0; i < N; ++i) { // ?
				for (int j = 0; j < N; ++j) {
					if (i == j) continue;
					dist[i][j] = EuclideanDistance(city_coords[i].first, city_coords[i].second, city_coords[j].first, city_coords[j].second); // distance
					attractiveness[i][j] = 1.0 / dist[i][j];
				}
			}

			ifs.close();
		}
		else {
			throw "Error: File Cannot Open!\n";
		}
	}

	void Print() {
		for (int i = 0; i < N; ++i)
			cout << i << " | " << city_coords[i].first << ' ' << city_coords[i].second << endl;

		cout << endl;

		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				cout << dist[i][j] << ' ';
			}
			cout << endl;
		}

		cout << endl;
	}

	double TourCost(vector<int>& trail) { // function to get distance from start trail to end trail (which is from last city to start city)
		double tourCost = 0.0; // start
		int n = trail.size() - 1;

		for (int i = 0; i < n; ++i)
			tourCost += dist[trail[i]][trail[i + 1]];

		tourCost += dist[trail[n]][trail[0]];
		return tourCost;
	}

	void print(vector<int>& solution) {
		cout << "City Index Passed: ";
		for (int i = 0; i < solution.size(); ++i)
			cout << solution[i] << ' ';
		cout << solution[0] << endl;
	}
};

WorldData world;

struct Ant {
	vector<int> trail;
	set<int> next_city;
	double alpha;
	double beta;
	int start_city_index;

	Ant(double a, double b) {
		alpha = a;
		beta = b;

		start_city_index = CityRandom(); // making ant start at random position

		trail.push_back(start_city_index);	// push start index

		for (int i = 0; i < world.N; ++i)
			if (i != start_city_index)
				next_city.insert(i);
	}

	int CityRandom() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> distrib(0, world.N - 1);
		return distrib(gen);
	}

	void Reset() {
		trail.clear();
		trail.push_back(start_city_index);

		for (int i = 0; i < world.N; ++i)
			if (i != start_city_index)
				next_city.insert(i);
	}

	void Save() {
		double tourCost = world.TourCost(trail);
		double save = 100 / tourCost;
		int n = trail.size() - 1;

		for (int i = 0; i < n; ++i)
			world.time[trail[i]][trail[i + 1]] += save;

		world.time[trail[n]][trail[0]] += save;
	}

	vector<int> Stop() {
		Save();
		vector<int> temp = trail;
		Reset();
		return temp;
	}

	double getRand() {
		return (rand() / (RAND_MAX + 1.0));
	}

	double ProbabilityIJ(int i, int j, double norm) {
		double p = (pow(world.time[i][j], alpha)) * (pow(world.attractiveness[i][j], beta));
		p /= norm;
		return p;
	}

	double ProbabilityNorm(int currentCity) {
		double norm = 0.0;

		for (set<int>::iterator i = next_city.begin(); i != next_city.end(); ++i)
			norm += (pow(world.time[currentCity][*i], alpha)) * (pow(world.attractiveness[currentCity][*i], beta));

		return norm;
	}

	void ObjectiveFunction() {
		int currentCity = trail[trail.size() - 1];
		double norm = ProbabilityNorm(currentCity);
		double prob, get_prob;

		bool go_to_next_city = false;

		double highestProb = 0;
		double cityHighest = 0;

		for (set<int>::iterator i = next_city.begin(); i != next_city.end(); ++i) {
			prob = ProbabilityIJ(currentCity, *i, norm);

			if (prob > highestProb) {
				cityHighest = *i;
				highestProb = prob;
			}

			get_prob = getRand();

			if (get_prob <= prob) {
				go_to_next_city = true;
				trail.push_back(*i);
				next_city.erase(i);
				break;
			}
		}

		if (!go_to_next_city) {
			trail.push_back(cityHighest);
			next_city.erase(cityHighest);
		}
	}
};

class TSP_ACO {
public:
	int N;
	int A;
	vector<Ant> ant;
	double evaporation;
	double alpha;
	double beta;

	TSP_ACO(double a = 1, double b = 2, double e = 0.1) {
		alpha = a;
		beta = b;
		evaporation = e;
		N = world.N;
		A = 20;

		for (int i = 0; i < A; ++i)
			ant.push_back(Ant(alpha, beta));
	}

	void Solution() {

		cout << "Traveling Salesman Problem with Ant Colony Optimization" << endl;

		int generation = 1;
		int max_generation = log2(N + A);

		while (generation <= max_generation) {
			vector<int> solution;
			double minimal_path = 0, tour = 0;
			for (int i = 0; i < A; ++i) {

				for (int j = 0; j < N - 1; ++j)
					for (int k = 0; k < A; ++k)
						ant[k].ObjectiveFunction();

				for (int j = 0; j < A; ++j) {
					vector<int> p = ant[j].Stop();

					if (!solution.size()) {
						solution = p;
						minimal_path = world.TourCost(p);
						continue;
					}

					tour = world.TourCost(p);

					if (tour < minimal_path) {
						minimal_path = tour;
						solution = p;
					}
				}

				for (int j = 1; j < N; ++j)
					for (int k = 1; k < N; ++k)
						world.time[j][k] *= evaporation;
			}

			cout << "\nIteration #" << generation << endl;
			cout << "Minimal Distance: " << minimal_path << endl;
			world.print(solution);
			++generation;
		}
	}
};

int main(void) {
	srand(time(NULL));
	world.Print();
	TSP_ACO colony;
	colony.Solution();
	return 0;
}