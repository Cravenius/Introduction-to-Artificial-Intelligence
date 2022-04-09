#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <random>

using namespace std;

class GeneticAlgorithm01Knapsack {
private:
	struct item {
		string name;
		int weight, value;
		item(string n, int w, int v) {
			name = n;
			weight = w;
			value = v;
		}
	};

	vector<item> items;
	vector<vector<bool>> chromosomes, offspring;
	vector<int> fitness;
	unordered_map<int, int> fitness_count;

	const int Size = 30, Generation = 20; // size is total sample, generation is limit of parent
	int N, W, total_fitness;
	int mutation_ratio;
	int crossover_ratio;

	int GeneticUniformDistribution() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> distrib(0, 1);
		return distrib(gen);
	}

	int CutOff() {
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<int> distrib(0, N);
		return distrib(gen);
	}

public:
	GeneticAlgorithm01Knapsack(int mutate_ratio = 1, int limit = 0, int cross_ratio = 90) {
		// Initialize array items reading from a file - the data (volume and value) for each item.
		this->items.push_back(item("Bug Repellent", 2, 12));
		this->items.push_back(item("Tent", 11, 10));
		this->items.push_back(item("Stove", 4, 5));
		this->items.push_back(item("Clothes", 5, 11));
		this->items.push_back(item("Dried Foods", 3, 50)); // 5
		this->items.push_back(item("First Aid Kit", 3, 15));
		this->items.push_back(item("Flash Light", 2, 6));
		this->items.push_back(item("Novel", 2, 4));
		this->items.push_back(item("Rain Gear", 2, 5));
		this->items.push_back(item("Sleeping Bag", 3, 25)); // 10
		this->items.push_back(item("Water Filter", 1, 30));
		this->items.push_back(item("Lantern", 7, 10));

		this->fitness.resize(Size);
		this->N = items.size();
		this->W = limit;
		this->mutation_ratio = mutate_ratio;
		this->crossover_ratio = cross_ratio;
		this->total_fitness = 0;
	}

	~GeneticAlgorithm01Knapsack() {
		this->items.clear();
		this->chromosomes.clear();
		this->offspring.clear();
		this->fitness.clear();
		this->fitness_count.clear();
	}

	void GeneratePopulation() {
		// Initialize array items reading from a the data (volume and value) for each item.
		this->chromosomes.resize(Size); // create a population of 100
		for (int i = 0; i < Size; ++i) {
			this->chromosomes[i].resize(this->N, 0); // create chromosome (bitset) of item count (length)
			for (int j = 0; j < this->N; ++j)
				this->chromosomes[i][j] = GeneticUniformDistribution();
		}
	}

	void ComputeFitness() { // normal evaluate function
		this->total_fitness = 0;
		this->fitness_count.clear();

		for (int i = 0; i < this->Size; ++i) { // For each chromosome in the population
			int total_value = 0, total_weight = 0;

			for (int j = 0; j < this->N; ++j) { // For each item in the chromosome if it is included (bit = 1) in the knapsack, add its volumeand benefit to the total volumeand benefit
				if (this->chromosomes[i][j]) {
					total_weight += this->items[j].weight;
					total_value += this->items[j].value;
				}
			}

			if (total_weight > this->W) { // Is total volume >	W of the knapsack
				while (total_weight > this->W) {

					// Randomly choose items from the chromosome until we generate an item that is included in the knapsack(i.e.bit = 1)

					int r = rand() % this->N;

					while (this->chromosomes[i][r] == 0)
						r = rand() % this->N;

					this->chromosomes[i][r] = 0; // Remove this item from the knapsack (i.e. change bit = 0)
					total_weight -= this->items[r].weight;
					total_value -= this->items[r].value;
				}
			}

			this->fitness[i] = total_value;
			++this->fitness_count[this->fitness[i]];
			this->total_fitness += this->fitness[i];
		}
	}

	int ComputePercentage() {
		int max_cnt = 0;

		for (auto x : this->fitness_count)
			max_cnt = max(max_cnt, x.second);

		return max_cnt * 100 / Size;
	}

	int Fittest() {
		int max_fitness = INT32_MIN, index = -1;
		for (int i = 0; i < this->Size; ++i) {
			if (max_fitness < this->fitness[i]) {
				max_fitness = this->fitness[i];
				index = i;
			}
		}

		return index;
	}

	int SecondFittest(int fittest_index) {
		int max_second_fitness = INT32_MIN, index = -1;
		for (int i = 0; i < this->Size; ++i) {
			if (i != fittest_index && max_second_fitness < this->fitness[i]) {
				max_second_fitness = this->fitness[i];
				index = i;
			}
		}

		return index;
	}

	void TopCandidateParent() {
		int fittest_index = Fittest();
		this->offspring.push_back(this->chromosomes[fittest_index]);
		int second_fittest_index = SecondFittest(fittest_index);
		this->offspring.push_back(this->chromosomes[second_fittest_index]);
	}

	int RouletteWheelSelection() {
		// another selection method is Group Selection
		int fsum = 0; // Sum the total expected fitness (benefit) of all individuals in the population(we call it fsum).
		int limit = rand() % this->total_fitness; // Choose a random integer (limit) between 0 and Size.

		for (int i = 0; i < this->Size; ++i) {

			// Loop through the individuals in the population, summing up the expected values until the sum is greater than or equal to limit.
			fsum += this->fitness[i];
			if (fsum >= limit)
				return i;

			// The individual whose expected fitness puts the sum over this limit is the one that is selected to be a parent in the next generation.
		}

		return -1;
	}

	void Mutation(vector<bool>& chromosome) {
		for (int i = 0; i < this->N; ++i) {

			// mutation probability
			// you can use another function like
			// r = (rand() % 100 + 1) / 100
			// if (prob < mutation_probability)

			int probability = rand() % 1000;
			if (probability < this->mutation_ratio)
				chromosome[i] = !chromosome[i];
		}
	}

	void Crossover(int parent1, int parent2) {
		vector<bool> chromosome1 = this->chromosomes[parent1]; // 1st parent
		vector<bool> chromosome2 = this->chromosomes[parent2]; // 2nd parent

		// swap, if string we can use an substring function
		for (int i = 0; i < CutOff(); ++i)
			swap(chromosome1[i], chromosome2[i]);

		// Perform mutation on the chromosomes obtained
		// insert to offspring
		Mutation(chromosome1);
		Mutation(chromosome2);
		this->offspring.push_back(chromosome1);
		this->offspring.push_back(chromosome2);
	}

	void Reproduce() {
		TopCandidateParent(); // randomly select 2 chromosomes from the population (select the 2 best parent)
		while (this->offspring.size() < this->Size) {
			int chromosome_index1 = RouletteWheelSelection();
			int chromosome_index2 = RouletteWheelSelection();
			Crossover(chromosome_index1, chromosome_index2); // Perform crossover on the 2 chromosomes selected
		}
	}

	void Solution() {
		GeneratePopulation();

		int percentage = 0, current_generation = 0;
		while (percentage < this->crossover_ratio || current_generation < this->Generation) { // Does crossover_ratio have the same fit value ? && Is the number of generations greater than the limit ?

			if (current_generation) {
				this->chromosomes = this->offspring;
				this->offspring.clear();
			}

			// Calculate the fitness and volume of all chromosomes
			ComputeFitness();
			percentage = ComputePercentage(); // Check what percentage of the chromosomes in the population has the same fitness value
			++current_generation;

			// if almost extinct or out of generation limit
			if (percentage < this->crossover_ratio || current_generation < this->Generation) // does 90% of them not have the same fitness value ?
				Reproduce();
		}

		int solution_index = Fittest();
		cout << "Population: " << this->Size << endl;
		cout << "Number of Generation: " << this->Generation << endl;

		cout << "\nAll Chromosomes: " << endl;
		for (int i = 0; i < this->Size; ++i) {
			cout << "#(" << i + 1 << "): ";
			for (int j = 0; j < this->N; ++j) {
				cout << this->chromosomes[i][j];
			}
			cout << endl;
		}

		cout << "\nFittest Chromosome Value: " << this->fitness[solution_index] << endl;
		cout << "\nFittest Chromosome Index: " << solution_index << endl;

		cout << "Items Chosen: " << endl;
		for (int i = 0; i < this->N; ++i)
			if (this->chromosomes[solution_index][i])
				cout << "Item #" << i + 1 << endl;
	}
};

int main() {
	srand(time(NULL));
	GeneticAlgorithm01Knapsack solution(1, 21);
	solution.Solution();
}