#include <iostream>
#include <vector>
#include <ga/ga.h>
#include "tinyxml/tinyxml.h"

#include "newton.cpp"

#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>

using namespace std;

float objective(GAGenome &);
void fillData();
void getSummary(GARealGenome, double summary[3]);
double getSummaryCost(GARealGenome, double);
double *getSummary2(GARealGenome);
double getVk(int, vector <double>);
double getDk(int, vector <double>);
double getG(int, int);
double getB(int, int);
double getYCell(int, int, vector <double>);
double getFCell(int, vector <double>);
double calculation(GARealGenome);

struct node {
	int no;
	int code;
	bool isGen;
	bool isBalance;
	float Degree;
	float Pload;
	float Qload;
	float Pgen;
	float Qgen;
	float V;
	float Qmin;
	float Qmax;
	float I;
	double G, B;

	float C[3];
	float Pgenmax, Pgenmin, Qgenmax, Qgenmin;

	vector <node *> lines;
};

struct cost {
	float A, b, c;
};

struct line {
	node *from, *to;
	float R, X, b, t;
	double G, B;
};

node *getNode(int);
line *getLine(int, int);

vector <node> nodes;
vector <node *> genNodes;
vector <node *> notGenNodes;
node *balanceNode;
vector <line> lines;

double sumPload = 0;	// Потребляемая активная мощность
double sumQload = 0;	// Потребляемая реактивная мощность
int n;					// число узлов
int k;					// число генераторных узлов



int main(int argc, char** argv) {
    return 0;
	fillData();

	cout << endl << "*** Исходные данные ***";
	cout << endl << "Генераторные узлы: ";
	for (vector <node *>::iterator it = genNodes.begin(); it != genNodes.end(); ++it) {
		cout << (*(*it)).no << " ";
	}
	cout << endl << "Нагрузочные узлы: ";
	for (vector <node *>::iterator it = notGenNodes.begin(); it != notGenNodes.end(); ++it) {
		cout << (*(*it)).no << " ";
	}
	cout << endl << "Балансирующий узел: " << (*balanceNode).no;
	cout << endl << "Линии и из характеристики: ";
	for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
		cout << endl << "из " << (*(*it).from).no << " в " << (*(*it).to).no
				<< "; G=" << (*it).G << ", B=" << (*it).B;
	}
	cout << endl << "Характеристики узлов: ";
	for (vector <node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		cout << endl << "Узел №" << (*it).no
				<< ": Gkk=" << (*it).G << "; Bkk=" << (*it).B
				<< "; Pload=" << (*it).Pload << "; Qload=" << (*it).Qload;
	}



	n = nodes.size();
	k = genNodes.size();

	GAParameterList params;
	GASteadyStateGA::registerDefaultParameters(params);
	params.set(gaNnGenerations, 1000);
	params.set(gaNpopulationSize, 20);
	params.set(gaNpMutation, 0.3);
	params.set(gaNpCrossover, 0.8);

	params.set(gaNscoreFrequency, 10);
	params.set(gaNflushFrequency, 50);
	params.set(gaNselectScores, (int)GAStatistics::AllScores);
	params.set(gaNscoreFilename, "bog.dat");
	params.parse(argc, argv, gaFalse);

	GARealAlleleSetArray alleles;
	cout << endl << "Лимиты генераторных узлов: ";
	for (vector <node *>::iterator it = genNodes.begin(); it != genNodes.end(); ++it) {
		cout << endl << "узел №" << (*(*it)).no << " : " << (*(*it)).Pgenmin << " - " << (*(*it)).Pgenmax << " МВт";
		alleles.add((*(*it)).Pgenmin, (*(*it)).Pgenmax, GAAllele::INCLUSIVE, GAAllele::INCLUSIVE);
	}
	cout << endl;

	GARealGenome genome(alleles, objective);
	genome.crossover(GARealGenome::OnePointCrossover);

	GASteadyStateGA ga(genome);
	ga.parameters(params);
	GANoScaling scaling;
	ga.scaling(scaling);
	GATournamentSelector selector;
	ga.selector(selector);
	ga.evolve();

	cout << endl << "*** Результаты ***";
	cout << endl << "Нужная активная мощность: " << sumPload << " МВт";

	genome = ga.statistics().bestIndividual();
//	cout << endl << ga.statistics() << ga.statistics().bestIndividual() << endl;
	cout << endl << "Получившиеся мощности: ";
	for (int i = 0, size = genome.length(); i < size; ++i) {
		cout << endl << "узел №" << (*(genNodes[i])).no << " " << genome.gene(i) << " МВт";
	}
	double summary[3];
	getSummary(genome, summary);
	cout << endl << "Получившаяся суммарная мощность: " << summary[0] << " МВт";
	cout << endl << "Получившаяся стоимость: " << summary[2];

	cout << endl << "Расчёт системы: ";
	calculation(genome);

	cout << endl << endl;

	return 0;
}

node *getNode(int num) {
	node *ret;

	for (vector <node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it).no == num) {
			return &(*it);
		}
	}

	return ret;
}

line *getLine(int k, int m) {
	line ret;

	ret.G = 0;
	ret.B = 0;

	for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
		node *from = (*it).from;
		node *to = (*it).to;
		if (((*from).no == k && (*to).no == m) || ((*from).no == m && (*to).no == k)) {
			return &(*it);
		}
	}

	return &ret;
}

float objective(GAGenome & g) {
	GARealGenome& genome = (GARealGenome&) g;
	float value = 0.0;
	double sumCost = 0.0;
	double sumP = 0.0;
	double pBalancedNode = calculation(genome);

	cout << endl << "Расчёт мощности в балансирующем узле: " << pBalancedNode;
	genome.gene(0, pBalancedNode < 0 ? 0 : pBalancedNode);

	for (int i = 0, size = genome.length(); i < size; ++i) {
		double p = genome.gene(i);

		sumCost += p * p * (*genNodes[i]).C[0] + p * (*genNodes[i]).C[1] + (*genNodes[i]).C[2];
	}
	cout << endl << "Расчёт стоимости: " << sumCost;

	value = 1000000/sumCost;

	return value;
}

vector <double> P;

double calculation(GARealGenome genome) {
	vector <double> x0, ans;
	double pRest = 0;

	for (int i = 0; i < n * 2; ++i) {
		x0.push_back(i < n ? 0 : 1);
	}

	P.clear();
	for (int i = 0; i < k; ++i) {
		P.push_back(genome.gene(i));
		if (i != 0) {
			pRest += genome.gene(i);
		}
	}

//	cout << endl << getYCell(1, 1, x0);
	cout << endl << "Решение: " <<
			newton(x0, getFCell, getYCell, ans);

//	cout << endl;
//	for (vector <double>::iterator it = ans.begin(); it != ans.end(); ++it) {
//		cout << (*it) << ", ";
//	}

	double losses = 0;
	for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
		int k = (*(*it).from).no;
		int m = (*(*it).to).no;
		double Vk = ans[k + n - 1];
		double Vm = ans[m + n - 1];
		double Dk = ans[k - 1];
		double Dm = ans[m - 1];
		double Gkm = (*it).G;

		losses += Gkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dk - Dm));
	}

	cout << endl << "Потери: " << losses;
	cout << endl;

	return 100 * losses + sumPload - pRest;
}

double getVk(int nodeNumber, vector <double> x) {
	double ret = 0;
	node *currentNode = getNode(nodeNumber);

	if ((*currentNode).isGen) {
		ret = (*currentNode).V;
	} else if (nodeNumber == 1) {
		ret = 1;
	} else {
		for (int i = 0, size = notGenNodes.size(); i < size; ++i) {
			if ((*notGenNodes[i]).no == nodeNumber) {
				ret = x[i + n - 1];
				break;
			}
		}
	}

	return ret;
}

double getDk(int nodeNumber, vector <double> x) {
	double ret = 0;

	if (nodeNumber == 1) {
		ret = 0;
	} else {
		ret = x[nodeNumber - 2];
	}

	return ret;
}

double getYCell(int i, int j, vector <double> x) {
	double ret = 0;

	int kNodeNumber = i % n + 1;;
	int mNodeNumber = j % n + 1;;
	node *kNode = getNode(kNodeNumber);
	node *mNode = getNode(mNodeNumber);
	line *kmLine = getLine(kNodeNumber, mNodeNumber);;

	bool isForP = i < n;
	bool isDelta = j < n;
	bool isGen = (*kNode).isGen;

	double Vk = x[kNodeNumber + n - 1];
	double Vm = x[mNodeNumber + n - 1];
	double Dk = x[kNodeNumber - 1];
	double Dm = x[mNodeNumber - 1];
	double Bkk = (*kNode).B;
	double Gkk = (*kNode).G;
	double Bkm = (*kmLine).B;
	double Gkm = (*kmLine).G;

//	cout << endl << "DEBUG: i=" << i << ", j=" << j
//			<< ", kNodeNumber=" << kNodeNumber
//			<< ", mNodeNumber=" << mNodeNumber
//			<< endl
//			<< ", isGen=" << isGen
//			<< ", isForP=" << isForP
//			<< ", isDelta=" << isDelta
//			<< ", Vk=" << Vk
//			<< ", Vm=" << Vm
//			<< endl
//			<< ", Dk=" << Dk
//			<< ", Dm=" << Dm
//			<< ", Gkm=" << Gkm << ", Bkm=" << Bkm
//			<< endl;

	if (kNodeNumber == 1) {
		if (i == j) {
			ret = 1;
		} else {
			ret = 0;
		}

		return ret;
	} else if (isGen && ! isForP) {
		if (i == j) {
			ret = 1;
		} else {
			ret = 0;
		}

		return ret;
	}

	if (isForP) {
		if (isDelta) {
			if (kNodeNumber == mNodeNumber) {
				for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
					node *from = (*it).from;
					node *to = (*it).to;
					double Bkm = (*it).B;
					double Gkm = (*it).G;
					int k = (*from).no;
					int m = (*to).no;
					double Dm, Vm;

					if (k != kNodeNumber && m != kNodeNumber) {
						continue;
					} else if (k != kNodeNumber) {
						m = k;
					}

					Dm = x[m - 1];
					Vm = x[k + n - 1];

					ret += Vm * (Gkm * sin(Dk - Dm) - Bkm * cos(Dk - Dm));
				}
				ret *= Vk;
			} else {
				ret = Vk * Vm
						* (-Gkm * sin(Dk - Dm) + Bkm * cos(Dk - Dm));
			}
		} else {
			if (kNodeNumber == mNodeNumber) {
				ret -= 2 * Vk * Gkk;
				for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
					node *from = (*it).from;
					node *to = (*it).to;
					double Bkm = (*it).B;
					double Gkm = (*it).G;
					int k = (*from).no;
					int m = (*to).no;
					double Dm, Vm;

					if (k != kNodeNumber && m != kNodeNumber) {
						continue;
					} else if (k != kNodeNumber) {
						m = k;
					}

					Dm = x[m - 1];
					Vm = x[k + n - 1];

					ret -= Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
				}
			} else {
				ret = -Vk
						* (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
			}
		}
	} else {
		if (isDelta) {
			if (kNodeNumber == mNodeNumber) {
				for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
					node *from = (*it).from;
					node *to = (*it).to;
					double Bkm = (*it).B;
					double Gkm = (*it).G;
					int k = (*from).no;
					int m = (*to).no;
					double Dm, Vm;

					if (k != kNodeNumber && m != kNodeNumber) {
						continue;
					} else if (k != kNodeNumber) {
						m = k;
					}

					Dm = x[m - 1];
					Vm = x[k + n - 1];

					ret -= Vm * (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
				}
				ret *= Vk;
			} else {
				ret = Vk * Vm
						* (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
			}
		} else {
			if (kNodeNumber == mNodeNumber) {
				ret += 2 * Vk * Bkk;
				for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
					node *from = (*it).from;
					node *to = (*it).to;
					double Bkm = (*it).B;
					double Gkm = (*it).G;
					int k = (*from).no;
					int m = (*to).no;
					double Dm, Vm;

					if (k != kNodeNumber && m != kNodeNumber) {
						continue;
					} else if (k != kNodeNumber) {
						m = k;
					}

					Dm = x[m - 1];
					Vm = x[k + n - 1];

					ret += Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
				}
			} else {
				ret = Vk
						* (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
			}
		}
	}

	return ret;
}

double getFCell(int i, vector <double> x) {
	double ret = 0;

	int currentNodeNumber = i % n + 1;
	bool isForP = i < n;
	node *currentNode = getNode(currentNodeNumber);
	bool isGen = (*currentNode).isGen;

	double Vk = x[currentNodeNumber + n - 1];
	double Pk = -(*currentNode).Pload / 100;
	double Qk = (*currentNode).Qload / 100;
	double Gkk = (*currentNode).G;
	double Bkk = (*currentNode).B;
	double Dk = x[currentNodeNumber - 1];

	if (isGen) {
		for (int i = 0; i < k; ++i) {
			if ((*genNodes[i]).no == currentNodeNumber) {
				Pk += P[i] / 100;
				break;
			}
		}
	}

//	cout << endl << "size: " << x.size() << ", i: " << i << ", isGen: " << isGen
//			<< ", isForP: " << isForP << ", nodeNumber: "
//			<< currentNodeNumber << ", V: " << Vk
//			<< ", P: " << Pk
//			<< ", D: " << Dk;

	if (currentNodeNumber == 1) {
		if (isForP) {
			ret = x[0];
		} else {
			ret = x[n] - (*currentNode).V;
		}
		return ret;
	} else if (isGen && ! isForP) {
		ret = Vk - (*currentNode).V;
	} else {
		if (isForP) {
			ret += Pk - Vk * Vk * Gkk;
		} else {
			ret += Qk + Vk * Vk * Bkk;
		}

		for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
			node *from = (*it).from;
			node *to = (*it).to;
			double Bkm = (*it).B;
			double Gkm = (*it).G;
			int k = (*from).no;
			int m = (*to).no;
			double Dm, Vm;

			if (k != currentNodeNumber && m != currentNodeNumber) {
				continue;
			} else if (k != currentNodeNumber) {
				m = k;
			}

			Dm = x[m - 1];
			Vm = x[k + n - 1];

	//			cout << endl << (*from).no << " " << (*to).no << " " << m << " " << Dm;
			if (isForP) {
				ret -= Vk * Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
			} else {
				ret += Vk * Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
			}
		}
	}

	return ret;
}

double getG(int from, int to) {
	double ret = 0;

	if (from != to) {
		for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
			if (
					((*(*it).from).no == from && (*(*it).to).no == to)
					|| ((*(*it).from).no == to && (*(*it).to).no == from)
			) {
				ret = (*it).R / ((*it).R * (*it).R + (*it).X * (*it).X);
				break;
			}
		}
	} else {
		for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
			if ((*(*it).from).no == from || (*(*it).to).no == from) {
				ret += (*it).R / ((*it).R * (*it).R + (*it).X * (*it).X);
			}
		}
	}

	return ret;
}

double getB(int from, int to) {
	double ret = 0;

	if (from != to) {
		for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
			if (
					((*(*it).from).no == from && (*(*it).to).no == to)
					|| ((*(*it).from).no == to && (*(*it).to).no == from)
			) {
				ret = (*it).X / ((*it).R * (*it).R + (*it).X * (*it).X);
				break;
			}
		}
	} else {
		for (vector <line>::iterator it = lines.begin(); it != lines.end(); ++it) {
			if ((*(*it).from).no == from || (*(*it).to).no == from) {
				ret += (*it).X / ((*it).R * (*it).R + (*it).X * (*it).X);
			}
		}
	}

	return ret;
}

void getSummary(GARealGenome genome, double summary[3]) {
	for (int i = 0, size = genome.length(); i < size; ++i) {
		double p = genome.gene(i);
		double full = p;

		summary[0] += p;
		summary[1] += 0;
		summary[2] += full * full * (*genNodes[i]).C[0] + full * (*genNodes[i]).C[1] + (*genNodes[i]).C[2];
	}
}

double *getSummary2(GARealGenome genome) {
	double ret[] = {0, 0, 0};

	for (int i = 0, size = genome.length(); i < size; ++i) {
		double p = genome.gene(i);
		double full = p;

		ret[0] += p;
		ret[1] += 0;
		ret[2] += full * full * (*genNodes[i]).C[0] + full * (*genNodes[i]).C[1] + (*genNodes[i]).C[2];
	}

	return ret;
}

double getSummaryCost(GARealGenome genome, double pBalanceNode) {
	double ret = 0;

	for (vector <node *>::iterator it = genNodes.begin(); it != genNodes.end(); ++it) {
		double p = (*(*it)).isBalance ? pBalanceNode : 0;

		ret += p * p * (*(*it)).C[0] + p * (*(*it)).C[1] + (*(*it)).C[2];
	}

	return 0;
}

void fillData() {
	ifstream bus_data_file("data/input/bus_data.txt");
	ifstream cost_data_file("data/input/cost_cofficients.txt");
	ifstream power_limits_file("data/input/generator_limits.txt");
	ifstream line_data_file("data/input/line_data.txt");

	while( ! bus_data_file.eof()) {
		node tmp;
		char str[128];

		bus_data_file.getline(str, sizeof(str));
		sscanf(str, "%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
					&tmp.no, &tmp.code,
					&tmp.V, &tmp.Degree,
					&tmp.Pload,
					&tmp.Qload,
					&tmp.Pgen,
					&tmp.Qgen,
					&tmp.Qmin,
					&tmp.Qmax,
					&tmp.I
			);
		tmp.isGen = false;
		tmp.isBalance = false;
		tmp.G = tmp.B = 0;
		if (tmp.code == 2) {
			tmp.isGen = true;
		} else if (tmp.code == 1) {
			tmp.isGen = true;
			tmp.isBalance = true;
		}
		nodes.push_back(tmp);

		sumPload += tmp.Pload;
		sumQload += tmp.Qload;
	}

	for (vector <node>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
		if ((*it).isGen) {
			genNodes.push_back(&(*it));
		} else {
			notGenNodes.push_back(&(*it));
		}
		if ((*it).isBalance) {
			balanceNode = &(*it);
		}
	}

	while ( ! line_data_file.eof()) {
		char str[128];
		line l;

		line_data_file.getline(str, sizeof(str));
		int from, to;
		sscanf(str, "%d\t%d\t%f\t%f\t%f\t%f\n", &from, &to, &l.R, &l.X, &l.b, &l.t);
		l.from = getNode(from);
		l.to = getNode(to);
		l.G = l.R / (l.R * l.R + l.X * l.X);
		l.B = l.X / (l.R * l.R + l.X * l.X);
		(*l.from).G -= l.G;
		(*l.from).B -= l.B;
		(*l.to).G -= l.G;
		(*l.to).B -= l.B;
		(*l.from).lines.push_back(l.to);
		(*l.to).lines.push_back(l.from);
		lines.push_back(l);
	}

	int i = 0;
	while ( ! cost_data_file.eof()) {
		char str[128];

		cost_data_file.getline(str, sizeof(str));
		sscanf(str, "%f\t%f\t%f\n", &(*genNodes[i]).C[0], &(*genNodes[i]).C[1], &(*genNodes[i]).C[2]);
		++i;
	}

	i = 0;
	while ( ! power_limits_file.eof()) {
		char str[128];

		power_limits_file.getline(str, sizeof(str));
		sscanf(str, "%f\t%f\t%f\t%f\n", &(*genNodes[i]).Pgenmin, &(*genNodes[i]).Pgenmax, &(*genNodes[i]).Qgenmin, &(*genNodes[i]).Qgenmax);
		++i;
	}


}
