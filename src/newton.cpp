#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

template<typename T> T abs(T x) { return x < 0 ? -x : x; }

// @todo
const int INF = 1000000000;
const double EPS = 1e-9;
const double EPS1 = 1e-3;
const int NMAX = 200;

int gauss(vector < vector<double> > a, vector<double> b, vector <double> & ans);
int newton(vector <double> x, double (* getFCell)(int i, vector <double>), double (* getYCell)(int i, int j, vector <double>), vector <double> & ans);
vector < vector <double> > getY(vector <double> x, double (* getYCell)(int i, int j, vector <double>));
double getYCell(int i, int j, vector <double> x);
vector <double> getF(vector <double> x, double (* getFCell)(int i, vector <double>));
double getFCell(int i, vector <double> x);
double getEPS(vector <double> x);

//int main() {
//	vector <double> x0, ans;
//	x0.push_back(1);
//	x0.push_back(0);
//	x0.push_back(0);
//
//	newton(x0, getFCell, getYCell, ans);
//
//	return 0;
//}

int newton(
		vector <double> &x0,
		double (* getFCell)(int i, vector <double>),
		double (* getYCell)(int i, int j, vector <double>)
) {
	bool check = true;
	int ret = 0;
	int i = 0;
	int n = x0.size();

	while(check) {
		vector <double> z;
		vector <double> f;
		vector < vector <double> > y;

		f = getF(x0, getFCell);
		y = getY(x0, getYCell);

		if ((ret = gauss(y, f, z)) != 1) {
			check = false;
		}

		cout << endl << "************" << endl << "Шаг №" << i;
		cout << endl << "Y:";
		for (int j = 0; j < n; ++j) {
			cout << endl;
			for (int k = 0; k < n; ++k) {
				cout << y[j][k] << " ";
			}
		}
		cout << endl << "F:";
		for (int j = 0; j < n; ++j) {
			cout << endl << f[j];
		}
		cout << endl << "Решение на шаге:" << ret << endl;
		for (int j = 0; j < n; ++j) {
			cout << "x" << j << " = " << x0[j] << "; ";
		}
		cout << endl << "Решение линейной системы: " << endl;
		for (int j = 0; j < n; ++j) {
			cout << "z" << j << " = " << z[j] << "; ";
		}
		cout << endl << "Погрешность: " << getEPS(z);
		cout << endl << "************" << endl;

		if (getEPS(z) < EPS1) {
			check = false;
			ret = 1;
		} else if (++i > NMAX) {
			check = false;
			ret = 0;
		} else {
			for (int j = 0; j < n; ++j) {
				if (abs(z[j]) < EPS) {
					z[j] = 0;
				}
				x0[j] -= z[j];
				if (abs(x0[j]) < EPS) {
					x0[j] = 0;
				}
			}
		}
	}

	return ret;
}

double getEPS(vector <double> x) {
	double ret = abs(x[0]);

	// @todo iterator
	for (int i = 1, n = x.size(); i < n; ++i) {
		if (abs(x[i]) > ret) {
			ret = abs(x[i]);
		}
	}

	return ret;
}

vector < vector <double> > getY(vector <double> x, double (* getYCell)(int i, int j, vector <double>)) {
	int n = (int) x.size();
	vector < vector <double> > ret;

	for (int i = 0; i < n; ++i) {
		vector <double> row;

		for (int j = 0; j < n; ++j) {
			double val = (* getYCell)(i, j, x);

			row.push_back(abs(val) < EPS ? 0 : val);
		}
		ret.push_back(row);
	}

	return ret;
}

/*
double getYCell(int i, int j, vector <double> x) {
	double ret = 0;

	if (i == 0) {
		ret = 2 * x[j];
	} else if (i == 1) {
		if (j == 0) {
			ret = 4 * x[j];
		} else if (j == 1) {
			ret = 2 * x[j];
		} else {
			ret = -4;
		}
	} else if (i == 2) {
		if (j == 0) {
			ret = 6 * x[j];
		} else if (j == 1) {
			ret = -4;
		} else {
			ret = 2 * x[j];
		}
	}

	return ret;
}
*/


vector <double> getF(vector <double> x, double (* getFCell)(int i, vector <double>)) {
	int n = (int) x.size();
	vector <double> ret;

	for (int i = 0; i < n; ++i) {
		double val = (* getFCell)(i, x);

		ret.push_back(abs(val) < EPS ? 0 : val);
	}

	return ret;
}

/*
double getFCell(int i, vector <double> x) {
	double ret = 0;

	if (i == 0) {
		ret = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1;
	} else if (i == 1) {
		ret = 2 * x[0] * x[0] + x[1] * x[1] - 4 * x[2];
	} else if (i == 2){
		ret = 3 * x[0] * x[0] - 4 * x[1] + x[2] * x[2];
	}

	return ret;
}
*/

int gauss (vector < vector<double> > a, vector <double> b, vector <double> & ans) {
	int n = (int) a.size();
	int m = (int) a[0].size();

	for (int i = 0; i < n; ++i) {
		a[i].push_back(b[i]);
	}

	vector<int> where (m, -1);
	for (int col=0, row=0; col<m && row<n; ++col) {
		int sel = row;
		for (int i=row; i<n; ++i)
			if (abs (a[i][col]) > abs (a[sel][col]))
				sel = i;
		if (abs (a[sel][col]) < EPS)
			continue;
		for (int i=col; i<=m; ++i)
			swap (a[sel][i], a[row][i]);
		where[col] = row;

		for (int i=0; i<n; ++i)
			if (i != row) {
				double c = a[i][col] / a[row][col];
				for (int j=col; j<=m; ++j)
					a[i][j] -= a[row][j] * c;
			}
		++row;
	}

	ans.assign (m, 0);
	for (int i=0; i<m; ++i)
		if (where[i] != -1)
			ans[i] = a[where[i]][m] / a[where[i]][i];
	for (int i=0; i<n; ++i) {
		double sum = 0;
		for (int j=0; j<m; ++j)
			sum += ans[j] * a[i][j];
		if (abs (sum - a[i][m]) > EPS)
			return 0;
	}

	for (int i=0; i<m; ++i)
		if (where[i] == -1)
			return INF;
	return 1;
}
