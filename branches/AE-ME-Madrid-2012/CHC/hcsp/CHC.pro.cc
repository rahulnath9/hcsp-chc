#include "CHC.hh"

using std::cout;

skeleton CHC {
// StopCondition -------------------------------------------------------------------------------------

StopCondition::StopCondition() {
}

StopCondition::~StopCondition() {
}

// SetUpParams -----------------------------------------------------------

SetUpParams::SetUpParams(Operator_Pool& pool, Problem& pbm) :
	_seed(0), _timeout(60.0), _pbm(pbm), _independent_runs(0),
			_nb_evolution_steps(0), _population_size(0),
			_select_parents(6), // Selection of parents: Select all individuals. (fixed)
			_select_offsprings(7), // Selection of offspring: Select the best individuals. (fixed)
			_inter_operators(), _intra_operators(), _refresh_global_state(1),
			_synchronized(0), _check_asynchronous(1), _display_state(0), _pool(
					pool) {
}

Operator_Pool& SetUpParams::pool() const {
	return _pool;
}

istream& operator>>(istream& is, SetUpParams& setup) {
	char buffer[MAX_BUFFER]; // current line in the setup file
	char command[50];
	long op;
	int parameter;
	short int nb_section = 0;
	short int nb_io = 0;
	short int nb_param = 0;
	short int nb_LAN_param = 0;

	while (is.getline(buffer, MAX_BUFFER, '\n')) {
		sscanf(buffer, " %s ", command);
		if (!(strcmp(command, "General")))
			nb_section = 0;
		if (!(strcmp(command, "Selection-Parameters")))
			nb_section = 1;
		if (!(strcmp(command, "Intra-Operators")))
			nb_section = 2;
		if (!(strcmp(command, "Inter-Operators")))
			nb_section = 3;
		if (!(strcmp(command, "LAN-configuration")))
			nb_section = 4;

		op = -1;
		sscanf(buffer, " %ld%*s ", &op);
		if (op < 0)
			continue;
		switch (nb_section) {
		case 0:
			switch (nb_param) {
			case 0:
				setup.independent_runs(op);
				break;
			case 1:
				setup.nb_evolution_steps(op);
				break;
			case 2:
				setup.population_size(op);
				break;
			case 3:
				setup.display_state(op);
				break;
			case 4:
				setup.seed(op);
				break;
			case 5:
				setup.timeout(buffer);
				break;
			}
			nb_param++;
			break;
		case 1:
			setup.pool().selector(setup.select_offsprings()).setup(buffer);
			break;
		case 2:
			setup.pool().intra_operators().append(Intra_Operator::create(op));
			setup.pool().intra_operator(nb_io).setup(buffer);
			setup._intra_operators.append(new unsigned int(nb_io));
			nb_io++;
			break;
		case 3:
			setup._inter_operators.append(new unsigned int(op));
			setup.pool().inter_operator(op).setup(buffer);
			break;
		case 4:
			if (nb_LAN_param >= 3)
				break;
			switch (nb_LAN_param) {
			case 0:
				setup.refresh_global_state(op);
				break;
			case 1:
				setup.synchronized(op);
				break;
			case 2:
				assert(op>0);
				setup.check_asynchronous(op);
				break;
			}
			nb_LAN_param++;
			break;
		}
	}

	return is;
}

ostream& operator<<(ostream& os, const SetUpParams& setup) {
	os << "CONFIGURATION -------------------------------------------" << endl
			<< endl;
	os << "\t" << "Independent runs : " << setup.independent_runs() << endl
			<< "\t" << "Evolution steps: " << setup.nb_evolution_steps()
			<< endl << "\t" << "Size of Population: "
			<< setup.population_size() << endl;
	os << "\t" << "With combination between parents and offsprings" << endl;

	os << "\t" << "Display State: " << setup.display_state() << endl << endl
			<< "\t" << "Selections:" << endl << "\t" << "-----------" << endl
			<< endl << "\t" << "Selection parents    -> "
			<< setup.pool().selector(setup.select_parents()) << endl << "\t"
			<< "Selection offsprings -> " << setup.pool().selector(
			setup.select_offsprings()) << endl << endl << "\t"
			<< "Intra_Operators: " << endl << "\t" << "-----------" << endl
			<< endl;

	for (uint i = 0; i < setup.intra_operators_size(); i++)
		os << "\t" << (setup.pool().intra_operator(
				setup.intra_operator_index(i))) << endl;

	os << endl << "\t" << "Inter_Operators: " << endl << "\t" << "-----------"
			<< endl << endl;

	for (uint i = 0; i < setup.inter_operators_size(); i++)
		os << "\t" << "Operator: " << setup.pool().inter_operator(
				setup.inter_operator_index(i)) << endl;

	os << endl << "\t" << "LAN configuration:" << endl << "\t"
			<< "----------------------" << endl << endl << "\t"
			<< "Refresh global state in number of generations: "
			<< setup.refresh_global_state() << endl;

	if (setup.synchronized())
		os << "\t" << "Running in synchronous mode" << endl;
	else
		os << "\t" << "Running in asynchronous mode" << endl;
	os << "\t" << "Interval for checking asynchronous receptions: "
			<< setup.check_asynchronous() << endl << endl;

	os << "\t" << "Timeout: " << setup.timeout() << endl << endl;

	os << endl << endl
			<< "END CONFIGURATION -------------------------------------------"
			<< endl << endl;
	//	continue_question();
	return os;
}

const unsigned int SetUpParams::independent_runs() const {
	return _independent_runs;
}

const unsigned long SetUpParams::nb_evolution_steps() const {
	return _nb_evolution_steps;
}

const unsigned int SetUpParams::population_size() const {
	return _population_size;
}

const unsigned long SetUpParams::refresh_global_state() const {
	return _refresh_global_state;
}

const bool SetUpParams::synchronized() const {
	return _synchronized;
}

const unsigned int SetUpParams::check_asynchronous() const {
	return _check_asynchronous;
}

const bool SetUpParams::display_state() const {
	return _display_state;
}

void SetUpParams::seed(const unsigned long val) {
	_seed = val;
}

float SetUpParams::timeout() const {
	return _timeout;
}

void SetUpParams::timeout(const char* buffer) {
	sscanf(buffer, " %f %*s ", &_timeout);
}

const unsigned long SetUpParams::seed() const {
	return _seed;
}

void SetUpParams::independent_runs(const unsigned int val) {
	_independent_runs = val;
}

void SetUpParams::nb_evolution_steps(const unsigned long val) {
	_nb_evolution_steps = val;
}

void SetUpParams::population_size(const unsigned int val) {
	_population_size = val;
}

void SetUpParams::display_state(const bool val) {
	_display_state = val;
}

void SetUpParams::refresh_global_state(const unsigned long val) {
	_refresh_global_state = val;
}

void SetUpParams::synchronized(const bool val) {
	_synchronized = val;
}

void SetUpParams::check_asynchronous(const unsigned int val) {
	_check_asynchronous = val;
}

const unsigned int SetUpParams::select_parents() const {
	return _select_parents;
}

const unsigned int SetUpParams::select_offsprings() const {
	return _select_offsprings;
}

void SetUpParams::select_parents(const unsigned int val) {
	_select_parents = val;
}

void SetUpParams::select_offsprings(const unsigned int val) {
	_select_offsprings = val;
}

const unsigned int SetUpParams::intra_operator_index(const unsigned int index) const {
	return _intra_operators[index];
}

const unsigned int SetUpParams::intra_operators_size() const {
	return _intra_operators.size();
}

const unsigned int SetUpParams::inter_operator_index(const unsigned int index) const {
	return _inter_operators[index];
}

const unsigned int SetUpParams::inter_operators_size() const {
	return _inter_operators.size();
}

void SetUpParams::RefreshState(const StateCenter& _sc) const {
	_sc.set_contents_state_variable("_select_parents",
			(char *) &_select_parents, 1, sizeof(_select_parents));
	_sc.set_contents_state_variable("_select_offsprings",
			(char *) &_select_offsprings, 1, sizeof(_select_offsprings));
	_sc.set_contents_state_variable("_display_state", (char *) &_display_state,
			1, sizeof(bool));
}

void SetUpParams::UpdateFromState(const StateCenter& _sc) const {
	unsigned long nbytes, length;
	_sc.get_contents_state_variable("_select_parents",
			(char *) &_select_parents, nbytes, length);
	_sc.get_contents_state_variable("_select_offsprings",
			(char *) &_select_offsprings, nbytes, length);
	_sc.get_contents_state_variable("_display_state", (char *) &_display_state,
			nbytes, length);
}

SetUpParams::~SetUpParams() {
}

// Statistics ------------------------------------------------------

Statistics::Statistics() {
}

ostream& operator<<(ostream& os, const Statistics& stats) {
	os << "\n---------------------------------------------------------------"
			<< endl;
	os << "                   STATISTICS OF CURRENT TRIAL                   "
			<< endl;
	os << "------------------------------------------------------------------"
			<< endl;
	for (int i = 0; i < stats.stats_data.size(); i++) {
		os << endl << " Trial:	" << stats.stats_data[i].trial
				<< " Generation: " << stats.stats_data[i].nb_generation
				<< " Evaluation: " << stats.stats_data[i].nb_evaluation
				<< " Current best cost: " << stats.stats_data[i].best_cost
				<< " Global best cost: "
				<< stats.stats_data[i].global_best_cost << " Avg: "
				<< stats.stats_data[i].average_cost << " Std. Dev.: "
				<< stats.stats_data[i].standard_deviation;
	}

	os << endl
			<< "------------------------------------------------------------------"
			<< endl;
	return os;
}

Statistics& Statistics::operator=(const Statistics& stats) {
	stats_data = stats.stats_data;
	return *this;
}

void Statistics::update(const Solver& solver) {
	struct stat *new_stat = (struct stat *) malloc(sizeof(struct stat));

	new_stat->trial = solver.current_trial();
	new_stat->nb_generation = solver.current_iteration();
	new_stat->nb_evaluation = solver.current_evaluations();
	new_stat->average_cost = solver.current_average_cost();
	new_stat->standard_deviation = solver.current_standard_deviation();
	new_stat->best_cost = solver.current_best_cost();
	new_stat->global_best_cost = solver.global_best_cost();

	stats_data.append(*new_stat);
}

void Statistics::clear() {
	stats_data.remove();
}

Statistics::~Statistics() {
}

// Population ------------------------------------------------------

Population::Population(const Problem& pbm, const SetUpParams& setup) :
	_parents(setup.population_size()),
			_fitness_values(setup.population_size()), _new_parents(
					setup.population_size()), _offsprings(
					setup.population_size()), _setup(setup), _evaluations(0) {
	for (int i = 0; i < _parents.size(); i++) {
		_parents[i] = new Solution(pbm);
		_new_parents[i] = new Solution(pbm);
		_fitness_values[i].index = i;
		_fitness_values[i].change = true;
	}
	for (int i = 0; i < _offsprings.size(); i++)
		_offsprings[i] = new Solution(pbm);
}

void Population::Evaluate(Solution* sols, struct individual &_f) {
	if (_f.change) {
		_f.change = false;
		_f.fitness = sols->getFitness();
		_evaluations++;
	}
}

Population& Population::operator=(const Population& pop) {
	for (int i = 0; i < _parents.size(); i++) {
		*_parents[i] = *((pop.parents())[i]);
		_fitness_values[i] = pop._fitness_values[i];
		_evaluations = pop._evaluations;
	}
	return (*this);
}

istream& operator>>(istream& is, Population& population) {
	return is;
}

ostream& operator<<(ostream& os, const Population& population) {
	os << "---------------------------------------------------------------"
			<< endl;
	os << "                           PRESENT POPULATION                    "
			<< endl << endl;
	for (int i = 0; i < population._parents.size(); i++) {
		//os << *population._parents[i] << endl;
		population._parents[i]->show(os);
	}
	os << endl
			<< "---------------------------------------------------------------"
			<< endl;
	return os;
}

const SetUpParams& Population::setup() const {
	return _setup;
}

void Population::initialize(int mypid, int pnumber) {
	for (int i = 0; i < _parents.size(); i++) {
		_parents[i]->initialize(mypid, pnumber, i);
		_fitness_values[i].index = i;
		_fitness_values[i].change = true;
	}
	evaluate_parents();
}

void Population::evaluate_parents() {
	double upper_fitness = (infinity() * (-1));
	double lower_fitness = (infinity());
	double current_fitness;
	double cost = 0.0;

	for (int i = 0; i < _fitness_values.size(); i++) {
		Evaluate(_parents[_fitness_values[i].index], _fitness_values[i]);
		current_fitness = _fitness_values[i].fitness;

		if (current_fitness > upper_fitness) {
			_upper_cost = i;
			upper_fitness = current_fitness;
		}

		if (current_fitness < lower_fitness) {
			_lower_cost = i;
			lower_fitness = current_fitness;
		}

		cost += current_fitness;
	}

	_average_cost = cost / _fitness_values.size();
}

void Population::evaluate_offsprings() {
	int i = 0;

	_fitness_aux = Rarray<struct individual> (_parents.size()
			+ _offsprings.size());
	for (i = 0; i < _parents.size(); i++) {
		Evaluate(_parents[_fitness_values[i].index], _fitness_values[i]);
		_fitness_aux[i] = _fitness_values[i];
	}

	for (int j = i; (j - i) < _offsprings.size(); j++) {
		_fitness_aux[j].index = j;
		_fitness_aux[j].change = true;
		Evaluate(_offsprings[j - i], _fitness_aux[j]);
	}
}

void Population::evolution() {
	select_parents(); // selects individuals to apply operators

	// apply selected operators
	for (int i = 0; i < _setup.intra_operators_size(); i++)
		_setup.pool().intra_operator(_setup.intra_operator_index(i)).execute(
				_offsprings);

	evaluate_offsprings();
	select_offsprings(); // selects new individuals

	// Local search
	if (rand01() < 0.25) {
		int individual = rand_int(0, _parents.size() - 1);
		_parents[individual]->doLocalSearch();
	}

	evaluate_parents(); // calculates fitness of new individuals
}

void Population::interchange(const unsigned long current_generation,
		NetStream& channel) {
	// apply selected operators
	for (int i = 0; i < _setup.inter_operators_size(); i++)
		_setup.pool().inter_operator(_setup.inter_operator_index(i)).execute(
				(*this), current_generation, channel, _setup.synchronized(),
				_setup.check_asynchronous());
}

void Population::select_parents() {
	_setup.pool().selector(_setup.select_parents()).prepare(_fitness_values,
			false);
	struct individual ind;
	for (int i = 0; i < _offsprings.size(); i++) {
		ind = _setup.pool().selector(_setup.select_parents()).select_one(
				_parents, _offsprings, _fitness_values, 0, false);
		*_offsprings[i] = *_parents[ind.index];
	}
}

void Population::select_offsprings() {
	_setup.pool().selector(_setup.select_offsprings()).prepare(_fitness_aux,
			false);
	const int ps = _parents.size();
	Rarray<struct individual> aux(ps);

	for (int i = 0; i < ps; i++) {
		aux[i] = _setup.pool().selector(_setup.select_offsprings()).select_one(
				_parents, _offsprings, _fitness_aux, 0, false);
		if (aux[i].index < ps) {
			*_new_parents[i] = *_parents[aux[i].index];
			aux[i].index = i;
		} else {
			*_new_parents[i] = *_offsprings[aux[i].index - ps];
			aux[i].index = i;
		}
	}

	Solution *interchange;
	for (int i = 0; i < ps; i++) {
		interchange = _parents[i];
		_parents[i] = _new_parents[i]; // interchanges pointers to solutions ( NO solutions !! )
		_new_parents[i] = interchange;
	}

	for (int i = 0; i < ps; i++) {
		_fitness_values[i] = aux[i];
	}
}

const Rarray<Solution*>& Population::parents() const {
	return _parents;
}

const Rarray<Solution*>& Population::offsprings() const {
	return _offsprings;
}

Rarray<struct individual>& Population::fitness_values() {
	return _fitness_values;
}

unsigned int Population::upper_cost() const {
	return _upper_cost;
}

unsigned int Population::lower_cost() const {
	return _lower_cost;
}

unsigned int Population::evaluations() const {
	return _evaluations;
}

double Population::best_cost() const {
	if ((*_parents[0]).pbm().direction() == minimize)
		return _fitness_values[_lower_cost].fitness;
	else
		return _fitness_values[_upper_cost].fitness;
}

double Population::worst_cost() const {
	if ((*_parents[0]).pbm().direction() == minimize)
		return _fitness_values[_upper_cost].fitness;
	else
		return _fitness_values[_lower_cost].fitness;
}

Solution& Population::best_solution() const {
	if ((*_parents[0]).pbm().direction() == minimize)
		return *_parents[_fitness_values[_lower_cost].index];
	else
		return *_parents[_fitness_values[_upper_cost].index];
}

Solution& Population::worst_solution() const {
	if ((*_parents[0]).pbm().direction() == minimize)
		return *_parents[_fitness_values[_upper_cost].index];
	else
		return *_parents[_fitness_values[_lower_cost].index];
}

Solution& Population::solution(const unsigned int index) const {
	return *_parents[index];
}

double Population::fitness(const unsigned int index) const {
	return _fitness_values[index].fitness;
}

double Population::average_cost() const {
	return _average_cost;
}

double Population::standard_deviation() const {
	double standard = 0.0;
	for (int i = 0; i < _fitness_values.size(); i++)
		standard += pow((_fitness_values[i].fitness - _average_cost), 2);
	standard = sqrt(standard / (_fitness_values.size() - 1));
	return standard;
}

Population::~Population() {
	for (int i = 0; i < _parents.size(); i++)
		delete (_parents[i]);
	for (int i = 0; i < _offsprings.size(); i++)
		delete (_offsprings[i]);
	for (int j = 0; j < _new_parents.size(); j++)
		delete (_new_parents[j]);
}

// Intra_operator  --------------------------------------------------------------

Intra_Operator::Intra_Operator(const unsigned int _number_op) :
	_number_operator(_number_op), probability(NULL) {
}

unsigned int Intra_Operator::number_operator() const {
	return _number_operator;
}

Intra_Operator *Intra_Operator::create(unsigned int _number_op) {
	switch (_number_op) {
	case 0:
		return new Crossover();
		break;
	case 1:
		return new Diverge();
		break;
	default:
		return User_Operator::create(_number_op);
		break;
	}
}

ostream& operator<<(ostream& os, const Intra_Operator& intra) {
	switch (intra.number_operator()) {
	case 0:
		os << (Crossover&) intra;
		break;
	case 1:
		os << (Diverge&) intra;
		break;
	default:
		os << (User_Operator&) intra;
		break;
	}
	return os;
}

Intra_Operator::~Intra_Operator() {
}

//  Crossover:Intra_operator -------------------------------------------------------------

Crossover::Crossover() :
	Intra_Operator(0) {
	probability = new float[1];
	probability[0] = -1.0;
}

void Crossover::cross(Solution& sol1, Solution& sol2) const // dadas dos soluciones de la poblacion, las cruza
{
	if (DEBUG) cout << endl << "[DEBUG] Crossover::cross =========================================" << endl;

	timespec ts;

	if (TIMING) {
		clock_gettime(CLOCK_REALTIME, &ts);
	}

	int cant_tasks = sol1.length();

	float distancia_minima;
	distancia_minima = cant_tasks / CROSSOVER_DISTANCE;

	if (probability[0] <= 0.0) {
		probability[0] = distancia_minima;
	}

	int distancia = sol1.distanceTo(sol2);

	if (distancia > distancia_minima) {
		Solver::global_calls[TIMING_CROSS]++;

		/*int sol = 0;
		if (rand01() < 0.5) {
			//(sol1.getFitness() < sol2.getFitness()) {
			sol = 1;
		} else {
			sol = 2;
		}*/

		for (int taskId = 0; taskId < cant_tasks; taskId++) {
			if (rand01() <= CROSS_TASK) {
				int taskPosSol1, machineIdSol1;
				int taskPosSol2, machineIdSol2;

				machineIdSol1 = sol1.getTaskAssignment(taskId);
				machineIdSol2 = sol2.getTaskAssignment(taskId);

				if (machineIdSol1 != machineIdSol2) {
					//if (sol == 1) {
						sol2.moveTask(taskId, machineIdSol1);
					//} else {
						sol1.moveTask(taskId, machineIdSol2);
					//}
				}
			}
		}
	}

	if (TIMING) {
		timespec ts_end;
		clock_gettime(CLOCK_REALTIME, &ts_end);

		double elapsed;
		elapsed = ((ts_end.tv_sec - ts.tv_sec) * 1000000.0) + ((ts_end.tv_nsec
				- ts.tv_nsec) / 1000.0);
		Solver::global_timing[TIMING_CROSS] += elapsed;
	}
}

void Crossover::execute(Rarray<Solution*>& sols) const {
	for (int i = 0; i + 1 < sols.size(); i = i + 2)
		cross(*sols[i], *sols[i + 1]);
}

ostream& operator<<(ostream& os, const Crossover& cross) {
	os << "Crossover.";
	return os;
}

void Crossover::RefreshState(const StateCenter& _sc) const {
}

void Crossover::UpdateFromState(const StateCenter& _sc) {
	unsigned long nbytes, length;
	_sc.get_contents_state_variable("_parameter_select_new_pop",
			(char *) probability, nbytes, length);
}

void Crossover::setup(char line[MAX_BUFFER]) {
	int op;
	sscanf(line, " %d ", &op);
}

Crossover::~Crossover() {
	delete[] probability;
}

//  Diverge: Sub_operator -------------------------------------------------------------


RouletteWheel::RouletteWheel(const vector<double> values,
		bool maximizeDirection) :
	_values(), _minSelectionValues(), _maxSelectionValues(), _size(
			values.size()), _maximize(maximizeDirection), _overallValue(0) {

	//	if (DEBUG) cout << endl << "[DEBUG] RouletteWheel::RouletteWheel start" << endl;

	_values.reserve(_size + 1);
	_minSelectionValues.reserve(_size + 1);
	_maxSelectionValues.reserve(_size + 1);

	for (int i = 0; i < _size; i++) {
		_values.push_back(values[i]);

		if (_maximize) {
			_maxSelectionValues.push_back(values[i]);
		} else {
			_maxSelectionValues.push_back(1 / values[i]);
		}

		_overallValue += _maxSelectionValues[i];
	}

	if (_overallValue > MAXDOUBLE)
		_overallValue = MAXDOUBLE;

	double previous = 0.0;
	for (int i = 0; i < _size; i++) {
		_minSelectionValues[i] = previous;
		_maxSelectionValues[i] = (_maxSelectionValues[i] / _overallValue)
				+ previous;
		previous = _maxSelectionValues[i];
	}
}

RouletteWheel::~RouletteWheel() {

}

int RouletteWheel::drawOneByIndex() const {
	//	if (DEBUG) cout << endl << "[DEBUG] RouletteWheel::drawOneByIndex start" << endl;

	if (_values.size() > 1) {
		int selectedValueIndex = 0;

		double random_selected;
		random_selected = rand01();

		while (!((random_selected >= _minSelectionValues[selectedValueIndex])
				&& (random_selected < _maxSelectionValues[selectedValueIndex]))
				&& (selectedValueIndex < _size))

			selectedValueIndex++;

		return selectedValueIndex;
	} else {
		return 0;
	}
}

Diverge::Diverge() :
	Intra_Operator(1), _retryCount(0) {
	probability = new float[1];
}

void Diverge::diverge(const Rarray<Solution*>& sols, int bestSolutionIndex,
		float mutationProbability) {

	//	if (DEBUG) cout << endl << "[DEBUG] Diverge::diverge" << endl;

	for (int i = 0; i < sols.size(); i++) {
		if (i != bestSolutionIndex) {
			if (rand01() <= mutationProbability) {
				sols[i]->doMutate();
			}
		} else {
			sols[i]->doLocalSearch();
		}
	}
}

void Diverge::execute(Rarray<Solution*>& sols) const {
	//diverge(sols, -1, 1.0);
	assert(false);
	exit(-1);
}

ostream& operator<<(ostream& os, const Diverge& diverge) {
	os << "Diverge." << " Probability: " << diverge.probability[0];
	return os;
}

void Diverge::setup(char line[MAX_BUFFER]) {
	int op;
	sscanf(line, " %d %f ", &op, &probability[0]);
	assert(probability[0]>=0);
}

void Diverge::RefreshState(const StateCenter& _sc) const {
	_sc.set_contents_state_variable("_diverge_probability",
			(char *) probability, 1, sizeof(float));
}

void Diverge::UpdateFromState(const StateCenter& _sc) {
	unsigned long nbytes, length;
	_sc.get_contents_state_variable("_diverge_probability",
			(char *) probability, nbytes, length);
}

Diverge::~Diverge() {
	delete[] probability;
}

// Inter_operator -------------------------------------------------------------------

Inter_Operator::Inter_Operator(const unsigned int _number_op,
		const Direction dir) :
	_number_operator(_number_op), direction(dir), migration_rate(1),
			migration_size(1), migration_selection_1(0), migration_selection_2(
					0), migration_selection_conf_1(0),
			migration_selection_conf_2(0) {
}

unsigned int Inter_Operator::number_operator() const {
	return _number_operator;
}

void Inter_Operator::setup(char line[MAX_BUFFER]) {
	int op;
	int new_migration_rate = 1;
	int new_migration_size = 1;
	int new_migration_selection_1 = 0;
	int new_migration_selection_conf_1 = 0;
	int new_migration_selection_2 = 0;
	int new_migration_selection_conf_2 = 0;

	sscanf(line, " %d %d %d %d %d %d %d ", &op, &new_migration_rate,
			&new_migration_size, &new_migration_selection_1,
			&new_migration_selection_conf_1, &new_migration_selection_2,
			&new_migration_selection_conf_2);

	assert(new_migration_rate>0);
	assert(new_migration_size>0);
	assert(new_migration_selection_1>=0);
	assert(new_migration_selection_conf_1>=0);
	assert(new_migration_selection_2>=0);
	assert(new_migration_selection_conf_2>=0);

	migration_rate = new_migration_rate;
	migration_size = new_migration_size;
	migration_selection_1 = new_migration_selection_1;
	migration_selection_conf_1 = new_migration_selection_conf_1;
	migration_selection_2 = new_migration_selection_2;
	migration_selection_conf_2 = new_migration_selection_conf_2;
}

void Inter_Operator::RefreshState(const StateCenter& _sc) const {
	_sc.set_contents_state_variable("_migration_rate",
			(char *) &migration_rate, 1, sizeof(migration_rate));
	_sc.set_contents_state_variable("_migration_size",
			(char *) &migration_size, 1, sizeof(migration_size));
	_sc.set_contents_state_variable("_migration_selection_1",
			(char *) &migration_selection_1, 1, sizeof(migration_selection_1));
	_sc.set_contents_state_variable("_migration_selection_2",
			(char *) &migration_selection_2, 1, sizeof(migration_selection_2));
	_sc.set_contents_state_variable("_migration_selection_conf_1",
			(char *) &migration_selection_conf_1, 1,
			sizeof(migration_selection_conf_1));
	_sc.set_contents_state_variable("_migration_selection_conf_2",
			(char *) &migration_selection_conf_2, 1,
			sizeof(migration_selection_conf_2));
}

void Inter_Operator::UpdateFromState(const StateCenter& _sc) {
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_rate",
			(char *) &migration_rate, nitems, length);
	_sc.get_contents_state_variable("_migration_size",
			(char *) &migration_size, nitems, length);
	_sc.get_contents_state_variable("_migration_selection_1",
			(char *) &migration_selection_1, nitems, length);
	_sc.get_contents_state_variable("_migration_selection_2",
			(char *) &migration_selection_2, nitems, length);
	_sc.get_contents_state_variable("_migration_selection_conf_1",
			(char *) &migration_selection_conf_1, nitems, length);
	_sc.get_contents_state_variable("_migration_selection_conf_2",
			(char *) &migration_selection_conf_2, nitems, length);
}

ostream& operator<<(ostream& os, const Inter_Operator& inter) {
	switch (inter.number_operator()) {
	case 0:
		os << (Migration&) inter;
		break;
	}
	return os;
}

Inter_Operator::~Inter_Operator() {
}

// Migration ------------------------------------------------------------

Migration::Migration(const Direction dir) :
	Inter_Operator(0, dir) {
}

void Migration::execute(Population& pop,
		const unsigned long current_generation, NetStream& _netstream,
		const bool synchronized, const unsigned int check_asynchronous) const {

	Solution* solution_to_send;
	Solution* solution_received;
	Solution* solution_to_remplace;
	bool need_to_revaluate = false;

	int nb_proc = _netstream.pnumber(); // Get the number of processes running
	int mypid = _netstream.my_pid();

	int to = (mypid + 1) % nb_proc; // Source (from) and Target (to) of processes
	int from = (nb_proc + mypid - 1) % nb_proc;

	// process number 0 is only to store the global state
	if (to == 0)
		to = 1;
	if (from == 0)
		from = nb_proc - 1;

	_netstream << set_target(to) << set_source(from) << get_target(&to)
			<< get_source(&from);

	if ((current_generation % migration_rate) == 0 && (current_generation
			!= pop.setup().nb_evolution_steps())) // in this generation this operator have to be applied
	{
		timespec ts;

		if (TIMING) {
			clock_gettime(CLOCK_REALTIME, &ts);
		}

		Solver::global_calls[TIMING_MIGRATION]++;

		pop.setup().pool().selector(migration_selection_1).prepare(
				pop.fitness_values(), false);

		_netstream << pack_begin;
		for (int i = 0; i < migration_size; i++) {
			// select individual to send
			solution_to_send = pop.parents()[pop.setup().pool().selector(
					migration_selection_1).select_one(pop.parents(),
					pop.offsprings(), pop.fitness_values(),
					migration_selection_conf_1, false).index];

			_netstream << *solution_to_send;
		}
		_netstream << pack_end;

		if (synchronized) // synchronous mode: blocked until data are received
		{
			pop.setup().pool().selector(migration_selection_2).prepare(
					pop.fitness_values(), true);

			// RUSO: modifico para que procesos no queden bloqueados en migracion al final
			_netstream << set_source(MPI_ANY_SOURCE);

			int tipo = 0;
			_netstream._wait2(any, tipo);

			if (tipo == 1) {
				return;
			}
			// Fin Ruso

			_netstream << wait(packed);
			_netstream << pack_begin;

			for (uint i = 0; i < migration_size; i++) {
				// select individual to be remplaced
				struct individual ind;
				ind
						= pop.setup().pool().selector(migration_selection_2).select_one(
								pop.parents(), pop.offsprings(),
								pop.fitness_values(),
								migration_selection_conf_2, true);
				solution_to_remplace = pop.parents()[ind.index];
				solution_received = new Solution(solution_to_remplace->pbm());
				_netstream >> *solution_received;

				// remplace policy
				if ((solution_received->getFitness()
						<= solution_to_remplace->getFitness() && direction
						== minimize) || (solution_received->getFitness()
						>= solution_to_remplace->getFitness() && direction
						== maximize)) {
					need_to_revaluate = true;
					for (int j = 0; j < pop.parents().size(); j++) {
						if (pop.fitness_values()[j].index == ind.index) {
							pop.fitness_values()[j].change = true;
							*pop.parents()[ind.index] = *solution_received;
						}
					}
				}
				delete (solution_received);
			}
			_netstream << pack_end;

		}

		if (TIMING) {
			timespec ts_end;
			clock_gettime(CLOCK_REALTIME, &ts_end);

			double elapsed;
			elapsed = ((ts_end.tv_sec - ts.tv_sec) * 1000000.0) + ((ts_end.tv_nsec
					- ts.tv_nsec) / 1000.0);
			Solver::global_timing[TIMING_MIGRATION] += elapsed;
		}
	} // end if

	if (!synchronized && ((current_generation % check_asynchronous) == 0)) { // asynchronous mode: if there are not data, continue;
		// but, if there are data, i have to receive it
		int pending = false;
		_netstream._probe(packed, pending);
		if (pending) {
			pop.setup().pool().selector(migration_selection_2).prepare(
					pop.fitness_values(), true);

			_netstream << pack_begin;
			for (int i = 0; i < migration_size; i++) {
				pending = false;
				_netstream._probe(regular, pending);
				if (!pending)
					break;

				// select individual to be remplaced
				struct individual ind;
				ind
						= pop.setup().pool().selector(migration_selection_2).select_one(
								pop.parents(), pop.offsprings(),
								pop.fitness_values(),
								migration_selection_conf_2, true);
				solution_to_remplace = pop.parents()[ind.index];
				solution_received = new Solution(solution_to_remplace->pbm());
				_netstream >> *solution_received;

				// remplace policy
				if ((solution_received->getFitness()
						<= solution_to_remplace->getFitness() && direction
						== minimize) || (solution_received->getFitness()
						>= solution_to_remplace->getFitness() && direction
						== maximize)) {
					need_to_revaluate = true;
					for (int j = 0; j < pop.parents().size(); j++) {
						if (pop.fitness_values()[j].index == ind.index) {
							pop.fitness_values()[j].change = true;
							*pop.parents()[ind.index] = *solution_received;
						}
					}
				}
				delete (solution_received);
			} // end for
			_netstream << pack_begin;
		} // end if
	}

	if (need_to_revaluate)
		pop.evaluate_parents();
}

ostream& operator<<(ostream& os, const Migration& migration) {
	os << "Migration." << endl << "\t" << " Rate: " << migration.migration_rate
			<< endl << "\t" << " Size: " << migration.migration_size << endl;

	os << "\t" << " Selection 1: (" << migration.migration_selection_1 << ") ";
	if (migration.migration_selection_1 == 0)
		os << "random";
	if (migration.migration_selection_1 == 1)
		os << "tournament";
	if (migration.migration_selection_1 == 2)
		os << "roulette wheel";
	if (migration.migration_selection_1 == 3)
		os << "rank";
	if (migration.migration_selection_1 == 4)
		os << "best";
	if (migration.migration_selection_1 == 5)
		os << "worst";
	os << endl << "\t" << " Selection 1 Parameter: "
			<< migration.migration_selection_conf_1 << endl;

	os << "\t" << " Selection 2: (" << migration.migration_selection_2 << ") ";
	if (migration.migration_selection_2 == 0)
		os << "random";
	if (migration.migration_selection_2 == 1)
		os << "tournament";
	if (migration.migration_selection_2 == 2)
		os << "roulette wheel";
	if (migration.migration_selection_2 == 3)
		os << "rank";
	if (migration.migration_selection_2 == 4)
		os << "best";
	if (migration.migration_selection_2 == 5)
		os << "worst";
	os << endl << "\t" << " Selection 2 Parameter: "
			<< migration.migration_selection_conf_2;
	return os;
}

Migration::~Migration() {
}

/*
 Migration::Migration(const Direction dir) :
 Inter_Operator(0, dir) {
 }

 void Migration::execute(Population& pop,
 const unsigned long current_generation, NetStream& _netstream,
 const bool synchronized, const unsigned int check_asynchronous) const {

 timespec ts;

 if (TIMING) {
 clock_gettime(CLOCK_REALTIME, &ts);
 }

 Solver::global_calls[TIMING_MIGRATION]++;

 Solution* solution_to_send;
 Solution* solution_received;
 Solution* solution_to_remplace;
 bool need_to_revaluate = false;

 int nb_proc = _netstream.pnumber(); // Get the number of processes running
 int mypid = _netstream.my_pid();

 if (mypid != 0) {
 // Source (from) and Target (to) of processes
 //int to = (mypid + 1) % nb_proc;
 int from = mypid; //(nb_proc + mypid - 1) % nb_proc;
 //		if (from == 0)
 //			from = nb_proc - 1;

 if ((current_generation % migration_rate) == 0 && (current_generation
 != pop.setup().nb_evolution_steps())) // in this generation this operator have to be applied
 {
 for (int to = 1; to < nb_proc; to++) {
 if (to != from) {
 // process number 0 is only to store the global state
 //		if (to == 0)
 //			to = 1;

 _netstream << set_target(to) << set_source(from)
 << get_target(&to) << get_source(&from);

 pop.setup().pool().selector(migration_selection_1).prepare(
 pop.fitness_values(), false);

 _netstream << pack_begin;
 for (int i = 0; i < migration_size; i++) {
 // select individual to send
 solution_to_send
 = pop.parents()[pop.setup().pool().selector(
 migration_selection_1).select_one(
 pop.parents(), pop.offsprings(),
 pop.fitness_values(),
 migration_selection_conf_1, false).index];

 _netstream << *solution_to_send;
 }
 _netstream << pack_end;
 }
 }
 }

 _netstream << set_source(MPI_ANY_SOURCE);

 int pending = true;
 _netstream._probe(packed, pending);

 while (pending) {
 pop.setup().pool().selector(migration_selection_2).prepare(
 pop.fitness_values(), true);

 // RUSO: modifico para que procesos no queden bloqueados en migracion al final
 int tipo = 0;
 _netstream._wait2(any, tipo);

 if (tipo == 1) {
 return;
 }
 // Fin Ruso

 _netstream << wait(packed);
 _netstream << pack_begin;

 for (uint i = 0; i < migration_size; i++) {
 // select individual to be remplaced
 struct individual ind;
 ind
 = pop.setup().pool().selector(migration_selection_2).select_one(
 pop.parents(), pop.offsprings(),
 pop.fitness_values(),
 migration_selection_conf_2, true);
 solution_to_remplace = pop.parents()[ind.index];
 solution_received = new Solution(solution_to_remplace->pbm());
 _netstream >> *solution_received;

 // remplace policy
 if ((solution_received->getFitness()
 <= solution_to_remplace->getFitness() && direction
 == minimize) || (solution_received->getFitness()
 >= solution_to_remplace->getFitness() && direction
 == maximize)) {

 need_to_revaluate = true;

 for (int j = 0; j < pop.parents().size(); j++) {
 if (pop.fitness_values()[j].index == ind.index) {
 pop.fitness_values()[j].change = true;
 *pop.parents()[ind.index] = *solution_received;
 }
 }
 }
 delete (solution_received);
 }

 _netstream << pack_end;
 _netstream._probe(packed, pending);
 }
 }

 if (need_to_revaluate)
 pop.evaluate_parents();

 if (TIMING) {
 timespec ts_end;
 clock_gettime(CLOCK_REALTIME, &ts_end);

 double elapsed;
 elapsed = ((ts_end.tv_sec - ts.tv_sec) * 1000000.0) + ((ts_end.tv_nsec
 - ts.tv_nsec) / 1000.0);
 Solver::global_timing[TIMING_MIGRATION] += elapsed;
 }
 }
 */
/*
 ostream& operator<<(ostream& os, const Migration& migration) {
 os << "Migration." << endl << "\t" << " Rate: " << migration.migration_rate
 << endl << "\t" << " Size: " << migration.migration_size << endl;

 os << "\t" << " Selection 1: (" << migration.migration_selection_1 << ") ";
 if (migration.migration_selection_1 == 0)
 os << "random";
 if (migration.migration_selection_1 == 1)
 os << "tournament";
 if (migration.migration_selection_1 == 2)
 os << "roulette wheel";
 if (migration.migration_selection_1 == 3)
 os << "rank";
 if (migration.migration_selection_1 == 4)
 os << "best";
 if (migration.migration_selection_1 == 5)
 os << "worst";
 os << endl << "\t" << " Selection 1 Parameter: "
 << migration.migration_selection_conf_1 << endl;

 os << "\t" << " Selection 2: (" << migration.migration_selection_2 << ") ";
 if (migration.migration_selection_2 == 0)
 os << "random";
 if (migration.migration_selection_2 == 1)
 os << "tournament";
 if (migration.migration_selection_2 == 2)
 os << "roulette wheel";
 if (migration.migration_selection_2 == 3)
 os << "rank";
 if (migration.migration_selection_2 == 4)
 os << "best";
 if (migration.migration_selection_2 == 5)
 os << "worst";
 os << endl << "\t" << " Selection 2 Parameter: "
 << migration.migration_selection_conf_2;
 return os;
 }

 Migration::~Migration() {
 }*/

// Selection ------------------------------------------------------------

Selection::Selection(const Direction dir) :
	_number_selection(0), direction(dir) {
}

Selection::Selection(const unsigned int _number_sel, const Direction dir) :
	_number_selection(_number_sel), direction(dir) {
}

void Selection::prepare(Rarray<struct individual>& fitness_values,
		const bool remplace) {
}

struct individual Selection::select_one(const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int dummy, const bool remplace) const { // select a random individual
	return fitness_values[rand_int(0, fitness_values.size() - 1)];
}

unsigned int Selection::number_selection() const {
	return _number_selection;
}

ostream& operator<<(ostream& os, const Selection& sel) {
	switch (sel.number_selection()) {
	case 0:
		os << "Random Selection";
		break;
	case 1:
		os << (Selection_Tournament&) sel;
		break;
	case 2:
		os << (Selection_Roulette_Wheel&) sel;
		break;
	case 3:
		os << (Selection_Rank&) sel;
		break;
	case 4:
		os << (Selection_Best&) sel;
		break;
	case 5:
		os << (Selection_Worst&) sel;
		break;
	case 6:
		os << (Selection_Parents&) sel;
		break;
	case 7:
		os << (Selection_New_Population&) sel;
		break;
	}
	return os;
}

void Selection::setup(char line[MAX_BUFFER]) {
}

void Selection::RefreshState(const StateCenter& _sc) const {
}

void Selection::UpdateFromState(const StateCenter& _sc) {
}

Selection::~Selection() {
}

// Selection_Tournament----------------------------------------------------

Selection_Tournament::Selection_Tournament(const Direction dir) :
	Selection(1, dir) {
}

struct individual Selection_Tournament::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int tournament_size, const bool remplace) const {
	unsigned int best_sol = 0;
	double best_fitness = ((-1) * direction * infinity());
	unsigned int index;

	if (remplace)
		best_fitness = -1 * best_fitness;

	unsigned int new_tournament_size = tournament_size;
	if (tournament_size == 0)
		new_tournament_size = 1;

	for (int i = 0; i < new_tournament_size; i++) {
		index = rand_int(0, fitness_values.size() - 1);

		switch (direction) {
		case (minimize):
			if (((!remplace) && (fitness_values[index].fitness < best_fitness))
					|| ((remplace) && (fitness_values[index].fitness
							> best_fitness))) {
				best_sol = index;
				best_fitness = fitness_values[index].fitness;
			}
			break;
		case (maximize):
			if (((!remplace) && (fitness_values[index].fitness > best_fitness))
					|| ((remplace) && (fitness_values[index].fitness
							< best_fitness))) {
				best_sol = index;
				best_fitness = fitness_values[index].fitness;
			}
			break;
		}
	}

	return fitness_values[best_sol];
}

ostream& operator<<(ostream& os, const Selection_Tournament& sel) {
	os << "Tournament Selection.";
	return os;
}

Selection_Tournament::~Selection_Tournament() {
}

// Selection_Roulette_Wheel ---------------------------------------------------

Selection_Roulette_Wheel::Selection_Roulette_Wheel(const Direction dir) :
	Selection(2, dir) {
}

void Selection_Roulette_Wheel::prepare(
		Rarray<struct individual>& fitness_values, const bool remplace) {

	double overall_fitness = 0.0;

	// inverts fitness values to select less fitness individuals with a high probability
	if ((direction == maximize && (remplace)) || ((direction == minimize)
			&& (!(remplace)))) {
		// fitness assigned if the fitness value is 0 in this case
		double value_if_zero = MAXDOUBLE;
		unsigned int nb_zeros = 0;

		for (int i = 0; i < fitness_values.size(); i++) {
			if (fitness_values[i].fitness != 0)
				value_if_zero -= fitness_values[i].fitness;
			else
				nb_zeros++;
		}

		value_if_zero = value_if_zero / nb_zeros;

		// Warning !! if fitness is 0 (1/0 ?)
		for (int i = 0; i < fitness_values.size(); i++) {
			if (fitness_values[i].fitness != 0)
				fitness_values[i].sel_parameter = (1
						/ fitness_values[i].fitness);
			else
				fitness_values[i].sel_parameter = value_if_zero;
			overall_fitness += fitness_values[i].sel_parameter;
		}
	} else {
		for (int i = 0; i < fitness_values.size(); i++) {
			fitness_values[i].sel_parameter = fitness_values[i].fitness;
			overall_fitness += fitness_values[i].sel_parameter;
		}

	}

	if (overall_fitness > MAXDOUBLE)
		overall_fitness = MAXDOUBLE;

	// calculate relative fitness
	double previous = 0.0;
	for (int i = 0; i < fitness_values.size(); i++) {
		fitness_values[i].sel_parameter = (fitness_values[i].sel_parameter
				/ overall_fitness) + previous;
		previous = fitness_values[i].sel_parameter;
	}
}

struct individual Selection_Roulette_Wheel::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int dummy, const bool remplace) const {
	double random_selected = rand01();
	int i = 0;

	while (random_selected > fitness_values[i].sel_parameter)
		i++;

	return fitness_values[i];
}

ostream& operator<<(ostream& os, const Selection_Roulette_Wheel& sel) {
	os << "Roulette Wheel Selection.";
	return os;
}

Selection_Roulette_Wheel::~Selection_Roulette_Wheel() {
}

// Selection_Rank --------------------------------------------------

int lessF(const struct individual &i1, const struct individual &i2) {
	return i1.fitness < i2.fitness;
}

int greaterF(const struct individual &i1, const struct individual &i2) {
	return i1.fitness > i2.fitness;
}

Selection_Rank::Selection_Rank(const Direction dir) :
	Selection(3, dir) {
}

Selection_Rank::Selection_Rank(const unsigned int _number_sel,
		const Direction dir) :
	Selection(_number_sel, dir) {
}

void Selection_Rank::reset() {
}

void Selection_Rank::prepare(Rarray<struct individual>& fitness_values,
		const bool remplace) {
	reset();

	// sort individuals
	if (((direction == maximize) && (!(remplace))) || ((direction == minimize)
			&& (remplace)))
		fitness_values.sort(lessF);
	else
		fitness_values.sort(greaterF);
}

struct individual Selection_Rank::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int portion, const bool remplace) const {

	unsigned int new_portion = portion;
	if (portion == 0 || portion > 100)
		new_portion = 100;

	return fitness_values[rand_int(0, ((fitness_values.size() * new_portion)
			/ 100) - 1)];
}

ostream& operator<<(ostream& os, const Selection_Rank& sel) {
	os << "Rank-Ordered Selection.";
	return os;
}

Selection_Rank::~Selection_Rank() {
}

// Selection_Best --------------------------------------------------

Selection_Best::Selection_Best(const Direction dir) :
	Selection_Rank(4, dir), selection_best_position(0) {
}

struct individual Selection_Best::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int position, const bool remplace) const {
	int position_to_return = position - 1;
	if (position_to_return < 0) {
		position_to_return = selection_best_position;
		selection_best_position++;
	}

	position_to_return = (int) position_to_return % fitness_values.size();

	return fitness_values[position_to_return];
}

void Selection_Best::reset() {
	selection_best_position = 0;
}

ostream& operator<<(ostream& os, const Selection_Best& sel) {
	os << "Selection of best ordered individuals.";
	return os;
}

Selection_Best::~Selection_Best() {
}

// Selection_Worst --------------------------------------------------

Selection_Worst::Selection_Worst(const Direction dir) :
	Selection_Rank(5, dir), selection_worst_position(0) {
}

struct individual Selection_Worst::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int position, const bool remplace) const {
	int position_to_return = position - 1;
	if (position_to_return < 0) {
		position_to_return = selection_worst_position;
		selection_worst_position++;
	}

	position_to_return = (int) position_to_return % fitness_values.size();

	int index = (fitness_values.size() - 1) - position_to_return;
	return fitness_values[index];
}

void Selection_Worst::reset() {
	selection_worst_position = 0;
}

ostream& operator<<(ostream& os, const Selection_Worst& sel) {
	os << "Selection of worst ordered individuals.";
	return os;
}

Selection_Worst::~Selection_Worst() {
}

// Selection_Parents  ---------------------------------------------------------------------------------

Selection_Parents::Selection_Parents(const Direction dir) :
	Selection(6, dir), selection_position(0) {
}

Selection_Parents::~Selection_Parents() {
}

ostream& operator<<(ostream& os, const Selection_Parents& sel) {
	os << "CHC Selection: Parents Selection.";
	return os;
}

void Selection_Parents::prepare(Rarray<struct individual>& fitness_values,
		const bool remplace) // const
{
	selection_position = 0;
	int limit1, limit2;
	register int max = fitness_values.size();

	struct individual interchange;

	for (int i = 0; i < max * 3; i++) {
		limit1 = rand_int(0, max - 1);
		limit2 = rand_int(0, max - 1);
		interchange = fitness_values[limit1];
		fitness_values[limit1] = fitness_values[limit2];
		fitness_values[limit2] = interchange;
	}
}

struct individual Selection_Parents::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int param, const bool remplace) const {
	int index = selection_position;
	selection_position++;

	index = index % fitness_values.size();

	return fitness_values[index];
}

// Selection_New_Population ---------------------------------------------------------------------------

Selection_New_Population::Selection_New_Population(const Direction dir) :
	Selection(7, dir), diverge(NULL), d(-1) {
}

Selection_New_Population::~Selection_New_Population() {
	if (diverge != NULL)
		delete diverge;
}

ostream& operator<<(ostream& os, const Selection_New_Population& sel) {
	os << "CHC Selection: New Population Selection." << " Diverge Operator: "
			<< *sel.diverge;
	return os;
}

void Selection_New_Population::prepare(
		Rarray<struct individual>& fitness_values, const bool remplace) // const
{
	selection_position = 0;

	// sort individuals
	if (((direction == maximize) && (!(remplace))) || ((direction == minimize)
			&& (remplace)))
		fitness_values.sort(greaterF);
	else
		fitness_values.sort(lessF);
}

// Selecciona un hijo para apareamiento.
//
// @to_select_1: parents solutions
// @to_select_2: offsprings solutions
// @fitness_values: parents+offsprings fitnesses
// @fitness_values: 0
// @fitness_values: false
struct individual Selection_New_Population::select_one(
		const Rarray<Solution*>& to_select_1,
		const Rarray<Solution*>& to_select_2,
		const Rarray<struct individual>& fitness_values,
		const unsigned int param, const bool remplace) const {
	bool change = false;

	if (selection_position == 0) {
		for (int i = 0; i < to_select_1.size(); i++)
			if (fitness_values[i].index > (to_select_1.size() - 1))
				change = true;

		if (change)
			selection_position++;
		else
			selection_position--;
	}

	if (d == -1)
		d = to_select_1[0]->length() / 4;

	if (selection_position == -1) {
		d--;
		if (d < 0) {
			selection_position--;
			d = (int) (r * (1.0 - r) * to_select_1[0]->length());
		} else
			selection_position = 1;
	}

	if (selection_position == -2) {
		assert(diverge != NULL);
		((Diverge *) diverge)->diverge(to_select_1, fitness_values[0].index,
				(float) 1.0);

		for (int i = 0; i < fitness_values.size(); i++) {
			//FIX: bug!
			if (fitness_values[i].index < to_select_1.size()) {
				fitness_values[i].change = true;
			}
		}

		selection_position = 1;
	}

	if (selection_position > 0) {
		int index = selection_position - 1;
		selection_position++;
		return fitness_values[index];
	}
}

void Selection_New_Population::setup(char line[MAX_BUFFER]) {
	int div;
	float div_par;

	sscanf(line, " %f %d %f ", &r, &div, &div_par);
	assert(r>=0);

	sprintf(line, "%d %f ", div, div_par);

	diverge = Intra_Operator::create(div);
	assert(diverge != NULL);

	diverge->setup(line);
}

void Selection_New_Population::RefreshState(const StateCenter& _sc) const {
	_sc.set_contents_state_variable("_parameter_select_new_pop", (char *) &r,
			1, sizeof(float));
}

void Selection_New_Population::UpdateFromState(const StateCenter& _sc) {
	unsigned long nbytes, length;
	_sc.get_contents_state_variable("_parameter_select_new_pop", (char *) &r,
			nbytes, length);
}

// Operator_Pool ------------------------------------------------------------------------

Operator_Pool::Operator_Pool(const Problem& pbm) {
	// introduces all operators and selections in lists

	// Index to be chosen in setup file
	//-------------------------------------
	// The Intra_Operators are introduced dimanicly in setup

	_selectors.append(new Selection(pbm.direction())); // 0
	_selectors.append(new Selection_Tournament(pbm.direction())); // 1
	_selectors.append(new Selection_Roulette_Wheel(pbm.direction())); // 2
	_selectors.append(new Selection_Rank(pbm.direction())); // 3
	_selectors.append(new Selection_Best(pbm.direction())); // 4
	_selectors.append(new Selection_Worst(pbm.direction())); // 5
	_selectors.append(new Selection_Parents(pbm.direction())); // 6
	_selectors.append(new Selection_New_Population(pbm.direction())); // 7

	_inter_operators.append(new Migration(pbm.direction())); // 0
}

Intra_Operator& Operator_Pool::intra_operator(const unsigned int index) const {
	assert(index < _intra_operators.size());
	return _intra_operators[index];
}

Rlist<Intra_Operator>& Operator_Pool::intra_operators() const {
	return _intra_operators;
}

Selection& Operator_Pool::selector(const unsigned int index) const {
	assert(index < _selectors.size());
	return _selectors[index];
}

const Rlist<Selection>& Operator_Pool::selectors() const {
	return _selectors;
}

Inter_Operator& Operator_Pool::inter_operator(const unsigned int index) const {
	assert(index < _inter_operators.size());
	return _inter_operators[index];
}

const Rlist<Inter_Operator>& Operator_Pool::inter_operators() const {
	return _inter_operators;
}

Operator_Pool::~Operator_Pool() {
}

// Solver (superclasse)---------------------------------------------------

double Solver::global_timing[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
int Solver::global_calls[5] = { 0, 0, 0, 0, 0 };

Solver::Solver(const Problem& pbm, const SetUpParams& setup) :
	problem(pbm), params(setup), _stat(), _userstat(), _sc(),
			current_population(pbm, setup), best_cost((-1) * pbm.direction()
					* infinity()), worst_cost((-1) * best_cost), best_solution(
					problem), average_cost(0.0), standard_deviation(0.0),
			time_spent_in_trial(0.0), total_time_spent(0.0), start_trial(0.0),
			start_global(0.0), _current_trial("_current_trial", _sc),
			_current_iteration("_current_iteration", _sc),
			_current_evaluations("_current_evaluations", _sc),
			_current_best_solution("_current_best_solution", _sc),
			_current_best_cost("_current_best_cost", _sc), _current_worst_cost(
					"_current_worst_cost", _sc), _current_average_cost(
					"_current_average_cost", _sc), _current_standard_deviation(
					"_current_standard_deviation", _sc), _current_time_spent(
					"_current_time_spent", _sc), _best_solution_trial(
					"_best_sol_trial", _sc), _best_cost_trial(
					"_best_cost_trial", _sc), _worst_cost_trial(
					"_worst_cost_trial", _sc), _iteration_best_found_in_trial(
					"_iteration_best_found_in_trial", _sc),
			_evaluations_best_found_in_trial(
					"_evaluations_best_found_in_trial", _sc),
			_time_best_found_trial("_time_best_found_trial", _sc),
			_time_spent_trial("_time_spent_trial", _sc), _trial_best_found(
					"_trial_best_found", _sc), _iteration_best_found(
					"_iteration_best_found", _sc), _evaluations_best_found(
					"_evaluations_best_found", _sc), _global_best_solution(
					"_global_best_solution", _sc), _global_best_cost(
					"_global_best_cost", _sc), _global_worst_cost(
					"_global_worst_cost", _sc), _time_best_found(
					"_time_best_found", _sc), _crossover_probability(
					"_crossover_probability", _sc), _diverge_probability(
					"_diverge_probability", _sc), _migration_rate(
					"_migration_rate", _sc), _migration_size("_migration_size",
					_sc),
			_migration_selection_1("_migration_selection_1", _sc),
			_migration_selection_2("_migration_selection_2", _sc),
			_migration_selection_conf_1("_migration_selection_conf_1", _sc),
			_migration_selection_conf_2("_migration_selection_conf_2", _sc),
			_select_parents("_select_parents", _sc), _select_offsprings(
					"_select_offsprings", _sc), _parameter_select_new_pop(
					"_parameter_select_new_pop", _sc), _display_state(
					"_display_state", _sc) {
	current_trial(0);
	current_iteration(0);
	current_evaluations(0);
	current_best_solution( best_solution);
	current_best_cost( best_cost);
	current_worst_cost( worst_cost);
	current_average_cost( average_cost);
	current_standard_deviation( standard_deviation);
	current_time_spent( total_time_spent);
	best_solution_trial(best_solution);
	best_cost_trial(best_cost);
	worst_cost_trial(worst_cost);
	iteration_best_found_in_trial(0);
	evaluations_best_found_in_trial(0);
	time_best_found_trial( time_spent_in_trial);
	time_spent_trial(time_spent_in_trial);
	trial_best_found(0);
	iteration_best_found(0);
	evaluations_best_found(0);
	global_best_solution(best_solution);
	global_best_cost(best_cost);
	global_worst_cost(worst_cost);
	time_best_found(total_time_spent);

	float prob[MAX_PROB_PER_OP] = { 0.0 };
	crossover_probability(prob);
	diverge_probability(prob);

	char aux[] = "_user_op_probability";
	char nombre[30];
	for (int i = 0; i < MAX_USER_OP; i++) {
		sprintf(nombre, "%s%d", aux, i);
		_user_op_probability[i].set_name((char *) nombre);
		_sc.add(_user_op_probability[i]);
		user_op_probability(i, prob);
	}

	migration_rate(0);
	migration_size(0);
	migration_selection_1(0);
	migration_selection_2(0);
	migration_selection_conf_1(0);
	migration_selection_conf_2(0);
	select_parents(0);
	select_offsprings(0);
	parameter_select_new_pop(0.0);
	display_state(setup.display_state());
}

int Solver::pid() const {
	return 0;
}

bool Solver::end_trial() const {
	return _end_trial;
}

void Solver::end_trial(bool et) {
	_end_trial = et;
}

unsigned int Solver::current_trial() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_current_trial.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned long Solver::current_iteration() const {
	unsigned long value = 0;
	unsigned long nitems, length;
	_current_iteration.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned long Solver::current_evaluations() const {
	unsigned long value = 0;
	unsigned long nitems, length;
	_current_evaluations.get_contents((char *) &value, nitems, length);
	return value;
}

Solution Solver::current_best_solution() const {
	Solution sol(problem);
	unsigned long nitems, length;
	char data_stored[_current_best_solution.get_nitems()
			+ _current_best_solution.get_length()];
	_current_best_solution.get_contents(data_stored, nitems, length);
	sol.to_Solution(data_stored);
	return sol;
}

double Solver::current_best_cost() const {
	double value = 0.0;
	unsigned long nitems, length;
	_current_best_cost.get_contents((char *) &value, nitems, length);
	return value;
}

double Solver::current_worst_cost() const {
	double value = 0.0;
	unsigned long nitems, length;
	_current_worst_cost.get_contents((char *) &value, nitems, length);
	return value;
}

double Solver::current_average_cost() const {
	double value = 0.0;
	unsigned long nitems, length;
	_current_average_cost.get_contents((char *) &value, nitems, length);
	return value;
}

double Solver::current_standard_deviation() const {
	double value = 0.0;
	unsigned long nitems, length;
	_current_standard_deviation.get_contents((char *) &value, nitems, length);
	return value;
}

float Solver::current_time_spent() const {
	float value = 0.0;
	unsigned long nitems, length;
	_current_time_spent.get_contents((char *) &value, nitems, length);
	return value;
}

Solution Solver::best_solution_trial() const {
	Solution sol(problem);
	char data_stored[_best_solution_trial.get_nitems()
			+ _best_solution_trial.get_length()];
	unsigned long nitems, length;
	_best_solution_trial.get_contents(data_stored, nitems, length);
	sol.to_Solution(data_stored);
	return sol;
}

double Solver::best_cost_trial() const {
	double value = 0.0;
	unsigned long nitems, length;
	_best_cost_trial.get_contents((char *) &value, nitems, length);
	return value;
}

double Solver::worst_cost_trial() const {
	double value = 0.0;
	unsigned long nitems, length;
	_worst_cost_trial.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned int Solver::iteration_best_found_in_trial() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_iteration_best_found_in_trial.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned int Solver::evaluations_best_found_in_trial() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_evaluations_best_found_in_trial.get_contents((char *) &value, nitems,
			length);
	return value;
}

float Solver::time_best_found_trial() const {
	float value = 0.0;
	unsigned long nitems, length;
	_time_best_found_trial.get_contents((char *) &value, nitems, length);
	return value;
}

float Solver::time_spent_trial() const {
	float value = 0.0;
	unsigned long nitems, length;
	_time_spent_trial.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned int Solver::trial_best_found() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_trial_best_found.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned int Solver::iteration_best_found() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_iteration_best_found.get_contents((char *) &value, nitems, length);
	return value;
}

unsigned int Solver::evaluations_best_found() const {
	unsigned int value = 0;
	unsigned long nitems, length;
	_evaluations_best_found.get_contents((char *) &value, nitems, length);
	return value;
}

Solution Solver::global_best_solution() const {
	Solution sol(problem);
	char data_stored[_global_best_solution.get_nitems()
			+ _global_best_solution.get_length()];
	unsigned long nitems, length;
	_global_best_solution.get_contents(data_stored, nitems, length);
	sol.to_Solution(data_stored);
	return sol;
}

double Solver::global_best_cost() const {
	double value = 0.0;
	unsigned long nitems, length;
	_global_best_cost.get_contents((char *) &value, nitems, length);
	return value;
}

double Solver::global_worst_cost() const {
	double value = 0.0;
	unsigned long nitems, length;
	_global_worst_cost.get_contents((char *) &value, nitems, length);
	return value;
}

float Solver::time_best_found() const {
	float value = 0.0;
	unsigned long nitems, length;
	_time_best_found.get_contents((char *) &value, nitems, length);
	return value;
}

int Solver::display_state() const {
	int value = 0;
	unsigned long nitems, length;
	_display_state.get_contents((char *) &value, nitems, length);
	return value;
}

float *Solver::crossover_probability() const {
	float *current_probability = new float[MAX_PROB_PER_OP];
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_crossover_probability",
			(char *) &current_probability, nitems, length);
	return current_probability;
}

float *Solver::diverge_probability() const {
	float *current_probability = new float[MAX_PROB_PER_OP];
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_diverge_probability",
			(char *) &current_probability, nitems, length);
	return current_probability;
}

float *Solver::user_op_probability(const int index) const {
	float *current_probability = new float[MAX_PROB_PER_OP];
	unsigned long nitems, length;
	char aux[30] = "_user_op_probability";
	sprintf(aux, "%s%d", aux, index);

	_sc.get_contents_state_variable(aux, (char *) &current_probability, nitems,
			length);
	return current_probability;
}

unsigned int Solver::migration_rate() const {
	unsigned int rate = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_rate", (char *) &rate, nitems,
			length);
	return rate;
}

unsigned int Solver::migration_size() const {
	unsigned int size = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_size", (char *) &size, nitems,
			length);
	return size;
}

unsigned int Solver::migration_selection_1() const {
	unsigned int selection_1 = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_selection_1",
			(char *) &selection_1, nitems, length);
	return selection_1;
}

unsigned int Solver::migration_selection_2() const {
	unsigned int selection_2 = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_selection_2",
			(char *) &selection_2, nitems, length);
	return selection_2;
}

unsigned int Solver::migration_selection_conf_1() const {
	unsigned int selection_conf_1 = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_selection_conf_1",
			(char *) &selection_conf_1, nitems, length);
	return selection_conf_1;
}

unsigned int Solver::migration_selection_conf_2() const {
	unsigned int selection_conf_2 = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_migration_selection_conf_2",
			(char *) &selection_conf_2, nitems, length);
	return selection_conf_2;
}

unsigned int Solver::select_parents() const {
	unsigned int select_parents = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_select_parents",
			(char *) &select_parents, nitems, length);
	return select_parents;
}

unsigned int Solver::select_offprings() const {
	unsigned int select_offsprings = 0;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_select_offsprings",
			(char *) &select_offsprings, nitems, length);
	return select_offsprings;
}

float Solver::parameter_select_new_pop() const {
	float parameter_select_new_pop;
	unsigned long nitems, length;
	_sc.get_contents_state_variable("_parameter_select_new_pop",
			(char *) &parameter_select_new_pop, nitems, length);
	return parameter_select_new_pop;
}

void Solver::current_trial(const unsigned int value) {
	_current_trial.set_contents((char *) &value, 1, sizeof(int));
}

void Solver::current_iteration(const unsigned long value) {
	_current_iteration.set_contents((char *) &value, 1, sizeof(long));
}

void Solver::current_evaluations(const unsigned long value) {
	_current_evaluations.set_contents((char *) &value, 1, sizeof(long));
}

void Solver::current_best_solution(const Solution& sol) {
	_current_best_solution.set_contents(sol.to_String(), 1, sol.size());
}

void Solver::current_best_cost(const double value) {
	_current_best_cost.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::current_worst_cost(const double value) {
	_current_worst_cost.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::current_average_cost(const double value) {
	_current_average_cost.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::current_standard_deviation(const double value) {
	_current_standard_deviation.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::current_time_spent(const float value) {
	_current_time_spent.set_contents((char *) &value, 1, sizeof(float));
}

void Solver::best_solution_trial(const Solution& sol) {
	_best_solution_trial.set_contents(sol.to_String(), 1, sol.size());
}

void Solver::best_cost_trial(const double value) {
	_best_cost_trial.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::worst_cost_trial(const double value) {
	_worst_cost_trial.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::iteration_best_found_in_trial(const unsigned int value) {
	_iteration_best_found_in_trial.set_contents((char *) &value, 1, sizeof(int));
}

void Solver::evaluations_best_found_in_trial(const unsigned int value) {
	_evaluations_best_found_in_trial.set_contents((char *) &value, 1,
			sizeof(int));
}

void Solver::time_best_found_trial(const float value) {
	_time_best_found_trial.set_contents((char *) &value, 1, sizeof(float));
}

void Solver::time_spent_trial(const float value) {
	_time_spent_trial.set_contents((char *) &value, 1, sizeof(float));
}

void Solver::trial_best_found(const unsigned int value) {
	_trial_best_found.set_contents((char *) &value, 1, sizeof(int));
}

void Solver::iteration_best_found(const unsigned int value) {
	_iteration_best_found.set_contents((char *) &value, 1, sizeof(int));
}

void Solver::evaluations_best_found(const unsigned int value) {
	_evaluations_best_found.set_contents((char *) &value, 1, sizeof(int));
}

void Solver::global_best_solution(const Solution& sol) {
	_global_best_solution.set_contents(sol.to_String(), 1, sol.size());
}

void Solver::global_best_cost(const double value) {
	_global_best_cost.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::global_worst_cost(const double value) {
	_global_worst_cost.set_contents((char *) &value, 1, sizeof(double));
}

void Solver::time_best_found(const float value) {
	_time_best_found.set_contents((char *) &value, 1, sizeof(float));
}

void Solver::display_state(const int value) {
	_display_state.set_contents((char *) &value, 1, sizeof(float));
}

void Solver::crossover_probability(const float *new_probability) {
	_sc.set_contents_state_variable("_crossover_probability",
			(char *) new_probability, MAX_PROB_PER_OP, sizeof(float));
}

void Solver::diverge_probability(const float *new_probability) {
	_sc.set_contents_state_variable("_diverge_probability",
			(char *) new_probability, MAX_PROB_PER_OP, sizeof(float));
}

void Solver::user_op_probability(const int index, const float *new_probability) {
	char aux[30] = "_user_op_probability";
	sprintf(aux, "%s%d", aux, index);

	_sc.set_contents_state_variable(aux, (char *) new_probability,
			MAX_PROB_PER_OP, sizeof(float));
}

void Solver::migration_rate(const unsigned int rate) {
	_sc.set_contents_state_variable("_migration_rate", (char *) &rate, 1,
			sizeof(int));
}

void Solver::migration_size(const unsigned int size) {
	_sc.set_contents_state_variable("_migration_size", (char *) &size, 1,
			sizeof(int));
}

void Solver::migration_selection_1(const unsigned int selection_1) {
	_sc.set_contents_state_variable("_migration_selection_1",
			(char *) &selection_1, 1, sizeof(int));
}

void Solver::migration_selection_2(const unsigned int selection_2) {
	_sc.set_contents_state_variable("_migration_selection_2",
			(char *) &selection_2, 1, sizeof(int));
}

void Solver::migration_selection_conf_1(const unsigned int selection_conf_1) {
	_sc.set_contents_state_variable("_migration_selection_conf_1",
			(char *) &selection_conf_1, 1, sizeof(int));
}

void Solver::migration_selection_conf_2(const unsigned int selection_conf_2) {
	_sc.set_contents_state_variable("_migration_selection_conf_2",
			(char *) &selection_conf_2, 1, sizeof(int));
}

void Solver::select_parents(const unsigned int selection) {
	_sc.set_contents_state_variable("_select_parents", (char *) &selection, 1,
			sizeof(int));
}

void Solver::select_offsprings(const unsigned int selection) {
	_sc.set_contents_state_variable("_select_offsprings", (char *) &selection,
			1, sizeof(int));
}

void Solver::parameter_select_new_pop(const float value) {
	_sc.set_contents_state_variable("_parameter_select_new_pop",
			(char *) &value, 1, sizeof(float));
}

Statistics& Solver::statistics() {
	return _stat;
}

UserStatistics& Solver::userstatistics() {
	return _userstat;
}

Population& Solver::population() {
	return current_population;
}

const SetUpParams& Solver::setup() const {
	return params;
}

const Problem& Solver::pbm() const {
	return problem;
}

void Solver::KeepHistory(const Solution& best_sol, const double best_cost,
		const double worst_cost, const float time_spent_in_trial,
		const float total_time_spent) {
	bool betterG = false;
	bool worseG = false;
	bool betterT = false;
	bool worseT = false;

	switch (problem.direction()) {
	case minimize:
		betterG = (best_cost < global_best_cost() || (best_cost
				== global_best_cost() && time_spent_in_trial
				< time_best_found()));
		worseG = (worst_cost > global_worst_cost());
		betterT = (best_cost < best_cost_trial() || (best_cost
				== best_cost_trial() && time_spent_in_trial
				< time_best_found_trial()));
		worseT = (worst_cost > worst_cost_trial());
		break;
	case maximize:
		betterG = (best_cost > global_best_cost() || (best_cost
				== global_best_cost() && time_spent_in_trial
				< time_best_found()));
		worseG = (worst_cost < global_worst_cost());
		betterT = (best_cost > best_cost_trial() || (best_cost
				== best_cost_trial() && time_spent_in_trial
				< time_best_found_trial()));
		worseT = (worst_cost < worst_cost_trial());
		break;
	}

	if (betterT) {
		best_solution_trial(best_sol);
		best_cost_trial(best_cost);
		time_best_found_trial(time_spent_in_trial);
		iteration_best_found_in_trial( current_iteration());
		evaluations_best_found_in_trial( current_evaluations());
		if (betterG) {
			global_best_solution(best_sol);
			global_best_cost(best_cost);
			time_best_found(time_spent_in_trial);
			trial_best_found( current_trial());
			iteration_best_found(current_iteration());
			evaluations_best_found(current_evaluations());
		}
	}

	if (worseT) {
		worst_cost_trial(worst_cost);
		if (worseG)
			global_worst_cost(worst_cost);
	}
}

StateCenter *Solver::GetState() {
	return &_sc;
}

void Solver::RefreshState() {
	current_best_cost( best_cost);
	current_worst_cost( worst_cost);
	current_average_cost( average_cost);
	current_standard_deviation( standard_deviation);
	current_time_spent( total_time_spent);
	time_spent_trial( time_spent_in_trial);
	current_best_solution( best_solution);

	KeepHistory(best_solution, best_cost, worst_cost, time_spent_in_trial,
			total_time_spent);
}

void Solver::RefreshCfgState() {
	for (int i = 0; i < params.pool().intra_operators().size(); i++)
		params.pool().intra_operator(i).RefreshState(_sc);
	for (int i = 0; i < params.pool().inter_operators().size(); i++)
		params.pool().inter_operator(i).RefreshState(_sc);
	for (int i = 0; i < params.pool().selectors().size(); i++)
		params.pool().selector(i).RefreshState(_sc);
	params.RefreshState(_sc);
}

void Solver::UpdateFromState() {
	best_solution = current_best_solution();
	best_cost = current_best_cost();
	worst_cost = current_worst_cost();
	average_cost = current_average_cost();
	standard_deviation = current_standard_deviation();
	total_time_spent = current_time_spent();
	time_spent_in_trial = time_spent_trial();
	KeepHistory(best_solution, best_cost, worst_cost, time_spent_in_trial,
			total_time_spent);
}

void Solver::UpdateFromCfgState() {
	for (int i = 0; i < params.pool().intra_operators().size(); i++)
		params.pool().intra_operator(i).UpdateFromState(_sc);
	for (int i = 0; i < params.pool().inter_operators().size(); i++)
		params.pool().inter_operator(i).UpdateFromState(_sc);
	for (int i = 0; i < params.pool().selectors().size(); i++)
		params.pool().selector(i).UpdateFromState(_sc);
	params.UpdateFromState(_sc);
}

void Solver::show_state() const {
	//if (!(current_iteration() % 100)){
	cout << endl
			<< " Current State ---------------------------------------------"
			<< endl;
	/*		cout << endl << "Selection parents    -> " << select_parents();
	 cout << endl << "Parameter of selection: " << parameter_select_parents();
	 cout << endl << "Selection offsprings -> " << select_offprings();
	 cout << endl << "Parameter of selection: " << parameter_select_offsprings() << endl;
	 cout << endl << "Crossover_probability: " << crossover_probability();
	 cout << endl << "Mutation_probability: " << mutation_probability();
	 cout << endl << "User_Operator_probability: " << user_op_probability(0);
	 cout << endl << "Migration_rate: " << migration_rate();
	 cout << endl << "Migration_size: " << migration_size();
	 cout << endl << "Migration_selection_1: " << migration_selection_1();
	 cout << endl << "Migration_selection_conf_1: " << migration_selection_conf_1();
	 cout << endl << "Migration_selection_2: " << migration_selection_2();
	 cout << endl << "Migration_selection_conf_2: " << migration_selection_conf_2() << endl;
	 */
	cout << endl << "Current trial: " << current_trial();
	cout << endl << "Current iteration: " << current_iteration();
	cout << endl << "Current evaluations: " << current_evaluations();
	cout << endl << "Current best cost: " << current_best_cost();
	cout << endl << "Current worst cost: " << current_worst_cost();
	cout << endl << "Current Average cost: " << current_average_cost();
	cout << endl << "Current Standard Deviation: "
			<< current_standard_deviation();
	cout << endl << endl << "Trial: ";
	cout << endl << "Best cost trial: " << best_cost_trial();
	cout << endl << "Worst cost trial: " << worst_cost_trial();
	cout << endl << "Iteration best found in trial: "
			<< iteration_best_found_in_trial();
	cout << endl << "Time best found trial: " << time_best_found_trial();
	cout << endl << "Time spent in trial: " << time_spent_trial();
	cout << endl << endl << "Global: ";
	cout << endl << "Global best cost: " << global_best_cost();
	cout << endl << "Global worst cost: " << global_worst_cost();
	cout << endl << "Trial best found: " << trial_best_found();
	cout << endl << "Iteration best found: " << iteration_best_found();
	cout << endl << "Time best found: " << time_best_found();
	cout << endl << endl << "Best Solution: " << endl;
	global_best_solution().show(cout);
	cout << endl << endl << "Current time spent (so far): "
			<< current_time_spent() << endl;
	//}
}

Solver::~Solver() {
	_sc.removeAll();
}

// Solver sequencial -----------------------------------------------------

Solver_Seq::Solver_Seq(const Problem& pbm, const SetUpParams& setup) :
	Solver(pbm, setup) {

	if (setup.seed() > 0) {
		if (DEBUG)
			cout << endl << "[DEBUG] Seed fijo " << setup.seed() << "." << endl;
		random_seed(setup.seed());
	} else {
		if (DEBUG)
			cout << endl << "[DEBUG] Seed aleatorio." << endl;
		random_seed(time(0));
	}

	_end_trial = true;
}

Solver_Seq::~Solver_Seq() {
}

void Solver_Seq::StartUp() {
	Population pop(problem, params);
	pop.initialize(0, 1); //Proceso 0, cantidad de procesos 1
	StartUp(pop);
}

void Solver_Seq::StartUp(const Population& pop) {
	start_trial = _used_time();
	start_global = total_time_spent;

	current_trial(current_trial() + 1);
	current_iteration(0);
	current_evaluations(pop.evaluations());

	// initialize state variables in the current trial

	Solution initial_solution(problem);

	time_spent_in_trial = 0.0;
	best_cost_trial((-1) * problem.direction() * infinity());
	worst_cost_trial((-1) * best_cost_trial());
	best_solution_trial(initial_solution);
	time_best_found_trial(0.0);

	current_population = pop;
	current_population.evaluate_parents();

	// gets current interesting values in the current population

	best_cost = current_population.best_cost();
	best_solution = current_population.best_solution();
	worst_cost = current_population.worst_cost();
	average_cost = current_population.average_cost();
	standard_deviation = current_population.standard_deviation();

	// refresh state with these values
	RefreshState();
	RefreshCfgState();

	UpdateFromCfgState();
	_stat.update(*this);
	_userstat.update(*this);

	if (display_state()) {
		show_state();
	}
}

void Solver_Seq::DoStep() {
	current_iteration(current_iteration() + 1);
	current_population.evolution();
	current_evaluations(current_population.evaluations());

	// gets current interesting values in the current population

	best_cost = current_population.best_cost();
	best_solution = current_population.best_solution();
	worst_cost = current_population.worst_cost();
	average_cost = current_population.average_cost();
	standard_deviation = current_population.standard_deviation();

	time_spent_in_trial = _used_time(start_trial);
	total_time_spent = start_global + time_spent_in_trial;

	// refresh state with these values
	RefreshState();
	RefreshCfgState();

	if ((current_iteration() % params.refresh_global_state()) == 0)
		UpdateFromCfgState();

	_stat.update(*this);
	_userstat.update(*this);

	if (display_state()) {
		show_state();
	}
}

void Solver_Seq::run() {
	while (current_trial() < params.independent_runs())
		run(params.nb_evolution_steps());
}

void Solver_Seq::run(const unsigned long int nb_generations) {
	StartUp();
	while ((current_iteration() < nb_generations) && !(terminateQ(problem,
			*this, params)))
		DoStep();

}

void Solver_Seq::run(const Population& pop,
		const unsigned long int nb_generations) {
	StartUp(pop);
	while ((current_iteration() < nb_generations) && !(terminateQ(problem,
			*this, params)))
		DoStep();

}

// Solver LAN ------------------------------------------------------------

Solver_Lan::Solver_Lan(Problem& pbm, const SetUpParams& setup, int argc,
		char **argv) :
	_best_solution_trial(pbm), Solver(pbm, setup), _netstream(),
	// Termination phase //
			final_phase(false), acum_evaluations(0), acum_iterations(0) {

	NetStream::init(argc, argv);
	mypid = _netstream.my_pid();
	pbm.setCurrentProcessId(mypid);

	if (setup.seed() > 0) {
		if (DEBUG)
			cout << endl << "[DEBUG] Seed fijo " << setup.seed() + (mypid + 1)
					<< "." << endl;
		random_seed(setup.seed() + (mypid + 1));
	} else {
		if (DEBUG)
			cout << endl << "[DEBUG] Seed aleatorio." << endl;
		random_seed(time(0) + (mypid + 1));
	}
}

Solver_Lan::~Solver_Lan() {
	NetStream::finalize();
}

int Solver_Lan::pid() const {
	return mypid;
}

NetStream& Solver_Lan::netstream() {
	return _netstream;
}

void Solver_Lan::StartUp() {
	//int my_pid = _netstream.my_pid();
	int pnumber = _netstream.pnumber();

	Population pop(problem, params);
	pop.initialize(pid(), pnumber);
	StartUp(pop);
}

void Solver_Lan::StartUp(const Population& pop) {
	_netstream << barrier;
	// Termination phase //
	final_phase = false;
	acum_evaluations = 0;
	acum_iterations = 0;

	start_trial = _used_time();
	start_global = total_time_spent;

	_end_trial = false;

	current_trial(current_trial() + 1);
	current_iteration(0);
	current_evaluations(pop.evaluations());

	// initialize state variables in the current trial

	Solution initial_solution(problem);

	time_spent_in_trial = 0.0;
	best_cost_trial((-1) * problem.direction() * infinity());
	worst_cost_trial((-1) * best_cost_trial());
	best_solution_trial(initial_solution);
	iteration_best_found_in_trial(0);
	time_best_found_trial(0.0);
	time_spent_trial(0.0);

	if (mypid != 0) {
		current_population = pop;
		current_population.evaluate_parents();

		// gets current interesting values in the current population

		best_cost = current_population.best_cost();
		best_solution = current_population.best_solution();
		worst_cost = current_population.worst_cost();
		average_cost = current_population.average_cost();
		standard_deviation = current_population.standard_deviation();

		// refresh state with these values
		RefreshState();
		RefreshCfgState();

		send_local_state_to( mypid);

		_stat.update(*this);
		_userstat.update(*this);

	}
}

void Solver_Lan::DoStep() {
	current_iteration(current_iteration() + 1);
	current_population.evolution();
	current_evaluations(current_population.evaluations());

	// Termination phase //
	_netstream << set_source(0);
	int pending;
	_netstream._probe(regular, pending);
	if (pending)
		final_phase = true;
	////////////////////////

	current_population.interchange(current_iteration(), _netstream);

	// gets current interesting values in the current population
	best_cost = current_population.best_cost();
	best_solution = current_population.best_solution();
	worst_cost = current_population.worst_cost();
	average_cost = current_population.average_cost();
	standard_deviation = current_population.standard_deviation();

	time_spent_in_trial = _used_time(start_trial);
	total_time_spent = start_global + time_spent_in_trial;

	// refresh state with these values
	RefreshState();
	RefreshCfgState();

	// in this iteration i have to send data about my local state to the global state
	if ((int) current_iteration() % params.refresh_global_state() == 0) {
		send_local_state_to( mypid);
		UpdateFromCfgState();
	}

	_stat.update(*this);
	_userstat.update(*this);

	if (display_state()) {
		show_state();
	}
}

void Solver_Lan::send_local_state_to(int _mypid) {
	_netstream << set_target(0);

	_netstream << pack_begin << _mypid << best_solution_trial()
			<< current_best_solution()

	<< current_trial() << current_iteration() << current_evaluations()
			<< best_cost_trial() << worst_cost_trial()
			<< time_best_found_trial() << current_best_cost()
			<< current_worst_cost() << current_average_cost()
			<< current_standard_deviation() << iteration_best_found_in_trial()
			<< evaluations_best_found_in_trial() << pack_end;
}

int Solver_Lan::receive_local_state() {
	int r_pid = 0;

	_netstream._wait(packed);

	_netstream << pack_begin >> r_pid >> _best_solution_trial >> best_solution

	>> _current_trial >> _current_iteration >> _current_evaluations
			>> _best_cost_trial >> _worst_cost_trial
			>> _time_best_found_in_trial >> best_cost >> worst_cost
			>> average_cost >> standard_deviation
			>> _iteration_best_found_in_trial
			>> _evaluations_best_found_in_trial << pack_end;

	return r_pid;
}

void Solver_Lan::check_for_refresh_global_state() // Executed in process with pid 0
{
	unsigned int nb_finalized_processes = 0;
	int received_pid;
	int nb_proc = _netstream.pnumber();

	_netstream << set_source(MPI_ANY_SOURCE);

	while (!_end_trial) {
		received_pid = 0;
		received_pid = receive_local_state();

		current_trial( _current_trial);

		// refresh the global state with received data ( a local state )
		current_iteration( _iteration_best_found_in_trial);
		current_evaluations( _evaluations_best_found_in_trial);
		KeepHistory(_best_solution_trial, _best_cost_trial, _worst_cost_trial,
				_time_best_found_in_trial, start_global
						+ _time_best_found_in_trial);

		// the process that has send data has finished the current trial
		if (received_pid == -1) {
			// Termination phase //
			if (!final_phase && terminateQ(problem, *this, params)) {
				acum_iterations = params.nb_evolution_steps()
						* nb_finalized_processes;

				acum_evaluations = acum_iterations * params.population_size()
						+ nb_finalized_processes * params.population_size();

				for (int i = 1; i < _netstream.pnumber(); i++) {
					_netstream << set_target(i);
					_netstream << 1;
				}

				final_phase = true;
			}
			nb_finalized_processes++;
			acum_iterations += _iteration_best_found_in_trial;
			acum_evaluations += _evaluations_best_found_in_trial;
		}
		if (nb_finalized_processes == nb_proc - 1)
			_end_trial = true;

		current_iteration( _current_iteration);
		current_evaluations( _current_evaluations);

		time_spent_in_trial = _used_time(start_trial);
		total_time_spent = start_global + time_spent_in_trial;
		RefreshState();
		RefreshCfgState();

		_stat.update(*this);
		//_userstat.update(*this);

		// display current global state
		if (display_state()) {
			show_state();
		}
	} // end while

	// Actualización de las estadísticas
	// Termination phase //
	iteration_best_found_in_trial(acum_iterations / (_netstream.pnumber() - 1));
	evaluations_best_found_in_trial(acum_evaluations / (_netstream.pnumber()
			- 1));

	bool betterG = false;
	double best_cost = best_cost_trial();
	switch (problem.direction()) {
	case minimize:
		betterG = (best_cost < global_best_cost() || (best_cost
				== global_best_cost() && time_best_found_trial()
				<= time_best_found()));
		break;
	case maximize:
		betterG = (best_cost > global_best_cost() || (best_cost
				== global_best_cost() && time_best_found_trial()
				<= time_best_found()));
		break;
	}

	if (betterG) {
		iteration_best_found( iteration_best_found_in_trial());
		evaluations_best_found( evaluations_best_found_in_trial());
	}

	RefreshState();
	RefreshCfgState();

	_stat.update(*this);
	_userstat.update(*this);

	// display the global state at the end of the current trial
	if (display_state())
		show_state();
}

void Solver_Lan::run() {
	while (current_trial() < params.independent_runs())
		run(params.nb_evolution_steps());
}

void Solver_Lan::run(const unsigned long int nb_generations) {
	StartUp();

	if (mypid != 0) {
		while (!final_phase && (current_iteration() < nb_generations)
				&& !(terminateQ(problem, *this, params))) {

			DoStep();
		}

		send_local_state_to(-1);

		cout << "[PID:" << mypid << "] current_iteration: " << current_iteration() << endl;
		
		/* MUESTRO LOS TIEMPOS RECOLECTADOS */
		/*if (TIMING) {*/
			//if (mypid == 1) {
				cout << "[TIMING_INIT]      (1)"
						<< Solver::global_timing[TIMING_INIT] << endl;
				cout << "[TIMING_CROSS]     ("
						<< Solver::global_calls[TIMING_CROSS] << ")"
						<< Solver::global_timing[TIMING_CROSS] << endl;
				cout << "[TIMING_MUTATE]    ("
						<< Solver::global_calls[TIMING_MUTATE] << ")"
						<< Solver::global_timing[TIMING_MUTATE] << endl;
				cout << "[TIMING_LS]        ("
						<< Solver::global_calls[TIMING_LS] << ")"
						<< Solver::global_timing[TIMING_LS] << endl;
				cout << "[TIMING_MIGRATION] ("
						<< Solver::global_calls[TIMING_MIGRATION] << ")"
						<< Solver::global_timing[TIMING_MIGRATION] << endl;
			//}
		//}
	} else {
		check_for_refresh_global_state();
	}

	_netstream << barrier;
	reset();
}

void Solver_Lan::run(const Population& pop,
		const unsigned long int nb_generations) {
	assert(false);

	StartUp(pop);
	if (mypid != 0) {
		while (!final_phase && (current_iteration() < nb_generations)
				&& !(terminateQ(problem, *this, params)))
			DoStep();
		send_local_state_to(-1);
	} else {
		check_for_refresh_global_state();
	}

	_netstream << barrier;
	reset();
}

void Solver_Lan::reset() {
	Solution left_solution(problem);
	int i;
	_netstream << set_source(MPI_ANY_SOURCE);
	if (mypid != 0) {
		int pendingr = false;
		int pendingp = false;
		do {
			pendingr = false;
			pendingp = false;
			_netstream._probe(regular, pendingr);
			_netstream._probe(packed, pendingp);
			if (pendingr)
				_netstream >> i;
			if (pendingp)
				_netstream << pack_begin >> left_solution << pack_end;
		} while (pendingr || pendingp);
	}
	_netstream << barrier;
}

// Solver WAN ------------------------------------------------------------

Solver_Wan::Solver_Wan(const Problem& pbm, const SetUpParams& setup, int argc,
		char **argv) :
	_best_solution_trial(pbm), Solver(pbm, setup), _netstream(),
	// Termination phase //
			final_phase(false), acum_evaluations(0), acum_iterations(0) {

	NetStream::init(argc, argv);
	mypid = _netstream.my_pid();
	random_seed(time(0) + (mypid + 1));
	//  random_seed(time(0) + (mypid+1));
}

Solver_Wan::~Solver_Wan() {
	NetStream::finalize();
}

int Solver_Wan::pid() const {
	return mypid;
}

NetStream& Solver_Wan::netstream() {
	return _netstream;
}

void Solver_Wan::StartUp() {
	Population pop(problem, params);
	pop.initialize(0, 1); // Proceso ID 0, cantidad de procesos 1
	StartUp(pop);
}

void Solver_Wan::StartUp(const Population& pop) {
	_netstream << barrier;

	// Termination phase //
	final_phase = false;
	acum_evaluations = 0;
	acum_iterations = 0;

	start_trial = _used_time();
	start_global = total_time_spent;

	_end_trial = false;

	current_trial(current_trial() + 1);
	current_iteration(0);
	current_evaluations(pop.evaluations());

	// initialize state variables in the current trial

	Solution initial_solution(problem);

	time_spent_in_trial = 0.0;
	best_cost_trial((-1) * problem.direction() * infinity());
	worst_cost_trial((-1) * best_cost_trial());
	best_solution_trial(initial_solution);
	iteration_best_found_in_trial(0);
	time_best_found_trial(0.0);
	time_spent_trial(0.0);

	if (mypid != 0) {
		current_population = pop;
		current_population.evaluate_parents();

		// gets current interesting values in the current population

		best_cost = current_population.best_cost();
		best_solution = current_population.best_solution();
		worst_cost = current_population.worst_cost();
		average_cost = current_population.average_cost();
		standard_deviation = current_population.standard_deviation();

		// refresh state with these values
		RefreshState();
		RefreshCfgState();

		send_local_state_to( mypid);

		_stat.update(*this);
		_userstat.update(*this);

	}
}

void Solver_Wan::DoStep() {
	current_iteration(current_iteration() + 1);
	current_population.evolution();
	current_evaluations(current_population.evaluations());

	// Termination phase //
	_netstream << set_source(0);
	int pending;
	_netstream._probe(regular, pending);
	if (pending)
		final_phase = true;
	////////////////////////

	current_population.interchange(current_iteration(), _netstream);

	// gets current interesting values in the current population

	best_cost = current_population.best_cost();
	best_solution = current_population.best_solution();
	worst_cost = current_population.worst_cost();
	average_cost = current_population.average_cost();
	standard_deviation = current_population.standard_deviation();

	time_spent_in_trial = _used_time(start_trial);
	total_time_spent = start_global + time_spent_in_trial;

	// refresh state with these values
	RefreshState();
	RefreshCfgState();

	// in this iteration i have to send data about my local state to the global state
	if ((int) current_iteration() % params.refresh_global_state() == 0) {
		send_local_state_to( mypid);
		UpdateFromCfgState();
	}

	_stat.update(*this);
	_userstat.update(*this);

	// if (display_state()) show_state();
}

void Solver_Wan::send_local_state_to(int _mypid) {
	_netstream << set_target(0);
	_netstream << pack_begin << _mypid << current_trial()
			<< current_iteration() << current_evaluations()
			<< best_cost_trial() << best_solution_trial()
			<< iteration_best_found_in_trial()
			<< evaluations_best_found_in_trial() << time_best_found_trial()
			<< worst_cost_trial() << current_best_cost()
			<< current_best_solution() << current_worst_cost()
			<< current_average_cost() << current_standard_deviation()
			<< pack_end;
}

int Solver_Wan::receive_local_state() {
	int r_pid = 0;

	_netstream._wait(packed);

	_netstream << pack_begin >> r_pid >> _current_trial >> _current_iteration
			>> _current_evaluations >> _best_cost_trial >> _best_solution_trial
			>> _iteration_best_found_in_trial
			>> _evaluations_best_found_in_trial >> _time_best_found_in_trial
			>> _worst_cost_trial >> best_cost >> best_solution >> worst_cost
			>> average_cost >> standard_deviation << pack_end;
	return r_pid;
}

void Solver_Wan::check_for_refresh_global_state() // Executed in process with pid 0
{
	unsigned int nb_finalized_processes = 0;
	int received_pid;
	int nb_proc = _netstream.pnumber();

	_netstream << set_source(MPI_ANY_SOURCE);

	while (!_end_trial) {
		received_pid = 0;
		received_pid = receive_local_state();

		current_trial( _current_trial);

		// refresh the global state with received data ( a local state )
		current_iteration( _iteration_best_found_in_trial);
		current_evaluations( _evaluations_best_found_in_trial);
		KeepHistory(_best_solution_trial, _best_cost_trial, _worst_cost_trial,
				_time_best_found_in_trial, start_global
						+ _time_best_found_in_trial);
		// the process that has send data has finished the current trial
		if (received_pid == -1) {
			// Termination phase //
			if (!final_phase && terminateQ(problem, *this, params)) {
				acum_iterations = params.nb_evolution_steps()
						* nb_finalized_processes;
				acum_evaluations = acum_iterations * params.population_size()
						+ nb_finalized_processes * params.population_size();
				for (int i = 1; i < _netstream.pnumber(); i++) {
					_netstream << set_target(i);
					_netstream << 1;
				}
				final_phase = true;
			}
			nb_finalized_processes++;
			acum_iterations += _iteration_best_found_in_trial;
			acum_evaluations += _evaluations_best_found_in_trial;
		}
		if (nb_finalized_processes == nb_proc - 1)
			_end_trial = true;

		current_iteration( _current_iteration);
		current_evaluations( _current_evaluations);

		time_spent_in_trial = _used_time(start_trial);
		total_time_spent = start_global + time_spent_in_trial;
		RefreshState();
		RefreshCfgState();

		_stat.update(*this);
		//_userstat.update(*this);

		// display current global state
		if (display_state())
			show_state();
	} // end while

	// Update Stats // Termination phase //
	iteration_best_found_in_trial(acum_iterations / (_netstream.pnumber() - 1));
	evaluations_best_found_in_trial(acum_evaluations / (_netstream.pnumber()
			- 1));

	bool betterG = false;
	double best_cost = best_cost_trial();
	switch (problem.direction()) {
	case minimize:
		betterG = (best_cost < global_best_cost() || (best_cost
				== global_best_cost() && time_best_found_trial()
				<= time_best_found()));
		break;
	case maximize:
		betterG = (best_cost > global_best_cost() || (best_cost
				== global_best_cost() && time_best_found_trial()
				<= time_best_found()));
		break;
	}

	if (betterG) {
		iteration_best_found( iteration_best_found_in_trial());
		evaluations_best_found( evaluations_best_found_in_trial());
	}

	RefreshState();
	RefreshCfgState();

	_stat.update(*this);
	_userstat.update(*this);

	// display the global state at the end of the current trial
	if (display_state())
		show_state();
}

void Solver_Wan::run() {
	while (current_trial() < params.independent_runs())
		run(params.nb_evolution_steps());
}

void Solver_Wan::run(const unsigned long int nb_generations) {
	StartUp();
	if (mypid != 0) {
		while (!final_phase && (current_iteration() < nb_generations)
				&& !(terminateQ(problem, *this, params)))
			DoStep();
		send_local_state_to(-1);
	} else {
		check_for_refresh_global_state();
	}

	_netstream << barrier;
	reset();
}

void Solver_Wan::run(const Population& pop,
		const unsigned long int nb_generations) {
	StartUp(pop);
	if (mypid != 0) {
		while (!final_phase && (current_iteration() < nb_generations)
				&& !(terminateQ(problem, *this, params)))
			DoStep();
		send_local_state_to(-1);
	} else {
		check_for_refresh_global_state();
	}

	_netstream << barrier;
	reset();
}

void Solver_Wan::reset() {
	Solution left_solution(problem);
	int i;
	_netstream << set_source(MPI_ANY_SOURCE);
	if (mypid != 0) {
		int pendingr = false;
		int pendingp = false;
		do {
			pendingr = false;
			pendingp = false;
			_netstream._probe(regular, pendingr);
			_netstream._probe(packed, pendingp);
			if (pendingr)
				_netstream >> i;
			if (pendingp)
				_netstream << pack_begin >> left_solution << pack_end;
		} while (pendingr || pendingp);

	}
	_netstream << barrier;
}

}
