#ifndef INC_REQ_CHC
#define INC_REQ_CHC
#include "CHC.hh"
#include <math.h>

using std::cout;

skeleton CHC {

// Problem ---------------------------------------------------------------

Problem::Problem(int tasks_count, int machines_count) :
	_taskCount(tasks_count), _machineCount(machines_count), _expectedTimeToCompute(NULL),
			_wrr_weights(), _makespan_weights(), _tasksPriorities(), _mypid(-1) {
}

// ===================================
// Serialización del problema.
// ===================================
ostream& operator<<(ostream& output, const Problem& pbm) {
	output << endl << endl << "Number of tasks: " << pbm._taskCount << endl
			<< "Number of machines: " << pbm._machineCount << endl << endl;
	return output;
}

// ===================================
// Deserialización del problema.
// ===================================
istream& operator>>(istream& input, Problem& pbm) {
	char buffer[MAX_BUFFER];

	//input.getline(buffer, MAX_BUFFER, '\n');
	//sscanf(buffer, "%d %d", &pbm._taskCount, &pbm._machineCount);

	//	cout << "[INFO] TaskCount: " << pbm._taskCount << endl;
	//	cout << "[INFO] MachineCount: " << pbm._machineCount << endl;

	// Inicializo las prioridades de las tareas.
	pbm._tasksPriorities.reserve(pbm._taskCount);

	int taskPriority;
	for (int taskPos = 0; taskPos < pbm._taskCount; taskPos++) {
		//input.getline(buffer, MAX_BUFFER, '\n');
		//sscanf(buffer, "%d", &taskPriority);
		//assert(taskPriority > 0);

		pbm._tasksPriorities.push_back(1);
	}

	// Inicializo toda la matriz de ETC.
	pbm._expectedTimeToCompute = new float*[pbm._taskCount];
	if (pbm._expectedTimeToCompute == NULL) {
		cout << "[ERROR] no se pudo reservar memoria para la matriz" << endl;
		show_message(7);
	}

	// Inicializo cada tarea del problema.
	for (int taskPos = 0; taskPos < pbm._taskCount; taskPos++) {
		// Por cada tarea creo una lista de maquinas.
		pbm._expectedTimeToCompute[taskPos] = new float[pbm._machineCount];

		if (pbm._expectedTimeToCompute[taskPos] == NULL) {
			cout
					<< "[ERROR] no se pudo reservar memoria para las máquinas de la tarea "
					<< taskPos << endl;
			show_message(7);
		}

		// Cargo el ETC de cada tarea en cada una de las máquinas.
		float aux;
		for (int machinePos = 0; machinePos < pbm._machineCount; machinePos++) {
			input.getline(buffer, MAX_BUFFER, '\n');

			sscanf(buffer, "%f", &aux);

			pbm._expectedTimeToCompute[taskPos][machinePos] = aux / 10000.0;

			assert(pbm._expectedTimeToCompute[taskPos][machinePos] >= 0);
		}
	}

	return input;
}

Problem& Problem::operator=(const Problem& pbm) {
	return *this;
}

bool Problem::operator==(const Problem& pbm) const {
	if (taskCount() != pbm.taskCount())
		return false;
	return true;
}

bool Problem::operator!=(const Problem& pbm) const {
	return !(*this == pbm);
}

Direction Problem::direction() const {
	return minimize;
}

int Problem::taskCount() const {
	return _taskCount;
}

int Problem::machineCount() const {
	return _machineCount;
}

int Problem::getBestTaskIdForMachine(int machineId) const {
	//TODO: Optimizar!
	int minTaskId = 0;
	for (int i = 1; i < taskCount(); i++) {
		if (expectedTimeToCompute(i, machineId) < expectedTimeToCompute(
				minTaskId, machineId)) {
			minTaskId = i;
		}
	}
	return minTaskId;
}

int Problem::getBestMachineForTaskId(int taskId) const {
	//TODO: Optimizar!
	int minMachineId = 0;
	for (int i = 1; i < machineCount(); i++) {
		if (expectedTimeToCompute(taskId, i) < expectedTimeToCompute(taskId,
				minMachineId)) {
			minMachineId = i;
		}
	}
	return minMachineId;
}

float Problem::expectedTimeToCompute(const int& task, const int& machine) const {
	assert(task >= 0);
	assert(task < _taskCount);
	assert(machine >= 0);
	assert(machine < _machineCount);
	return _expectedTimeToCompute[task][machine];
}

int Problem::taskPriority(const int& task) const {
	assert(task >= 0);
	assert(task < _taskCount);
	return _tasksPriorities[task];
}

void Problem::setPId(const int pid) {
	_mypid = pid;
}

void Problem::loadWeights(const vector<double> weights) {
	assert(weights.size() > 0);
	assert(weights.size() % 2 == 0);

	for (unsigned int i = 0; i < weights.size(); i = i + 2) {
		_makespan_weights.push_back(weights[i]);
		_wrr_weights.push_back(weights[i + 1]);
	}
}

double Problem::getWRRWeight() const {
	assert(_mypid >= 0);
	return getWRRWeight(_mypid);
}

double Problem::getMakespanWeight() const {
	assert(_mypid >= 0);
	return getMakespanWeight(_mypid);
}

double Problem::getWRRWeight(const int pid) const {
	if (pid == 0) {
		return _wrr_weights[0];
	} else {
		int index = (pid - 1) % _wrr_weights.size();
		return _wrr_weights[index];
	}
}

double Problem::getMakespanWeight(const int pid) const {
	if (pid == 0) {
		_makespan_weights[0];
	} else {
		int index = (pid - 1) % _makespan_weights.size();
		return _makespan_weights[index];
	}
}

Problem::~Problem() {
}

// Solution machine ------------------------------------------------------

SolutionMachine::SolutionMachine(const Problem& problem, int machineId) :
	_tasks(), _assignedTasks(), _machineId(machineId), _makespan(0.0), _awrr(
			0.0), _dirty(true), _pbm(problem) {

	_tasks.reserve(problem.taskCount());
}

SolutionMachine::~SolutionMachine() {
}

SolutionMachine& SolutionMachine::operator=(const SolutionMachine& machine) {
	_machineId = machine._machineId;

	_makespan = 0.0;
	_dirty = true;

	_tasks.clear();
	_tasks.reserve(machine._tasks.size());

	_assignedTasks.clear();

	for (int taskPos = 0; taskPos < machine._tasks.size(); taskPos++) {
		int taskId;
		taskId = machine.getTask(taskPos);

		_tasks.push_back(taskId);
		_assignedTasks[taskId] = NULL;
	}

	return *this;
}

int SolutionMachine::machineId() const {
	return _machineId;
}

void SolutionMachine::addTask(const int taskId) {
	_dirty = true;

	_tasks.push_back(taskId);
	_assignedTasks[taskId] = NULL;
}

void SolutionMachine::setTask(const int taskId, const int taskPos) {
	assert(taskPos >= 0);
	assert(taskPos < _tasks.size());

	_dirty = true;

	int removedTaskId = _tasks[taskPos];
	_assignedTasks.erase(removedTaskId);
	_assignedTasks[taskId] = NULL;

	_tasks.at(taskPos) = taskId;
}

void SolutionMachine::swapTasks(const int taskPos1, const int taskPos2) {
	assert(taskPos1 >= 0);
	assert(taskPos1 < _tasks.size());

	assert(taskPos2 >= 0);
	assert(taskPos2 < _tasks.size());

	_dirty = true;

	int taskId1 = _tasks[taskPos1];
	int taskId2 = _tasks[taskPos2];

	_tasks[taskPos1] = taskId2;
	_tasks[taskPos2] = taskId1;
}

int SolutionMachine::getTask(const int taskPos) const {
	assert(taskPos >= 0);
	assert(taskPos < _tasks.size());
	return _tasks[taskPos];
}

int SolutionMachine::countTasks() const {
	return _tasks.size();
}

bool SolutionMachine::hasTask(const int taskId) const {
	return _assignedTasks.count(taskId) == 1;
}

void SolutionMachine::showMap() const {
	for (map<int, void*>::const_iterator it = _assignedTasks.begin(); it
			!= _assignedTasks.end(); it++) {
		cout << (*it).first;
		cout << endl;
	}
}

int SolutionMachine::safeInsertTask(const int taskId, const int taskPos) {
	if (taskPos < _tasks.size()) {
		insertTask(taskId, taskPos);
		return taskPos;
	} else {
		addTask(taskId);
		return _tasks.size() - 1;
	}
}

void SolutionMachine::insertTask(const int taskId, const int taskPos) {
	assert(taskPos >= 0);
	assert(taskPos < _tasks.size());

	_dirty = true;

	_tasks.insert(_tasks.begin() + taskPos, taskId);
	_assignedTasks[taskId] = NULL;
}

void SolutionMachine::removeTask(const int taskPos) {
	assert(taskPos >= 0);
	assert(taskPos < _tasks.size());

	_dirty = true;

	int removedId = _tasks[taskPos];
	_assignedTasks.erase(removedId);

	_tasks.erase(_tasks.begin() + taskPos);
}

void SolutionMachine::emptyTasks() {
	_dirty = true;
	_assignedTasks.clear();
	_tasks.clear();
}

double SolutionMachine::getMakespan() {
	refresh();
	return _makespan;
}

double SolutionMachine::getAccumulatedWeightedResponseRatio() {
	refresh();
	return _awrr;
}

double SolutionMachine::getWeightedResponseRatio(const int taskPos) const {
	float wait_time = 0.0;

	for (int currentTaskPos = 0; currentTaskPos < taskPos; currentTaskPos++) {
		int currentTaskId;
		currentTaskId = getTask(currentTaskPos);

		wait_time += _pbm.expectedTimeToCompute(currentTaskId, machineId());
	}

	int taskId;
	taskId = getTask(taskPos);

	float compute_cost;
	compute_cost = _pbm.expectedTimeToCompute(taskId, machineId());

	float rr;
	rr = wait_time + compute_cost;

	float wrr;
	wrr = (_pbm.taskPriority(taskId) * rr);

	return wrr;
}

void SolutionMachine::refresh() {
	//_dirty = true;
	if (_dirty) {
		double partial_makespan = 0.0;
		double partial_awrr = 0.0;

		for (int taskPos = 0; taskPos < countTasks(); taskPos++) {
			int taskId;
			taskId = getTask(taskPos);

			double compute_cost;
			compute_cost = _pbm.expectedTimeToCompute(taskId, machineId());
			assert(compute_cost >= 0);

			double priority_cost;
			double rr;
			rr = partial_makespan + compute_cost;
			priority_cost = (_pbm.taskPriority(taskId) * rr);

			partial_makespan += compute_cost;
			partial_awrr += priority_cost;

		}

		_awrr = partial_awrr;
		_makespan = partial_makespan;

		_dirty = false;
	}
}

// Solution --------------------------------------------------------------

double Solution::_awrr_reference = 1.0;
double Solution::_makespan_reference = 1.0;

double Solution::getWRR_reference() {
	return Solution::_awrr_reference;
}

double Solution::getMakespan_reference() {
	return Solution::_makespan_reference;
}

Solution::Solution(const Problem& pbm) :
	_pbm(pbm), _machines(), _initialized(false) {
	_machines.reserve(pbm.machineCount());

	for (int machineId = 0; machineId < pbm.machineCount(); machineId++) {
		_machines.push_back(*(new SolutionMachine(pbm, machineId)));
	}
}

const Problem& Solution::pbm() const {
	return _pbm;
}

Solution::Solution(const Solution& sol) :
	_pbm(sol.pbm()) {
	*this = sol;
}

// ===================================
// Deserialización de la solución.
// ===================================
istream& operator>>(istream& is, Solution& sol) {
	//for (int i=0;i<sol.pbm().dimension();i++)
	//	is >> sol._var[i];

	assert(false);

	return is;
}

// ===================================
// Serialización de la solución.
// ===================================
ostream& operator<<(ostream& os, const Solution& sol) {
	os << endl;
	if (sol.isInitilized()) {
		/*for (int machineId = 0; machineId < sol.machines().size(); machineId++) {
		 os << "> machineId: " << machineId << endl;
		 //os << "  fitness: " << sol.fitnessByMachine(machineId) << endl;

		 for (int i = 0; i < sol.machines()[machineId].countTasks(); i++) {
		 os << "  taskPos: " << i;
		 os << " taskId: " << sol.machines()[machineId].getTask(i);
		 fprintf(stdout, " ETC: %f ", sol.pbm().expectedTimeToCompute(
		 sol.machines()[machineId].getTask(i), machineId));
		 fprintf(stdout, " WRR: %f ",
		 sol.machines()[machineId].getWeightedResponseRatio(i));
		 os << " priority: " << sol.pbm().taskPriority(
		 sol.machines()[machineId].getTask(i));
		 os << endl;
		 }
		 }*/
		//os << "* overall fitness: " << sol.fitness() << endl;
	} else {
		os << "> solution not inialized." << endl;
	}

	return os;
}

// ===================================
// Serialización de la solución.
// ===================================
NetStream& operator <<(NetStream& ns, const Solution& sol) {
	//	if (DEBUG) cout << endl << "[DEBUG] operator <<(NetStream& ns, Solution& sol)" << endl;

	int currentTask = 0;
	int currentItem = 0;

	int machineSeparator = -1;

	assert(sol.validate());

	for (int machineId = 0; machineId < sol.machines().size(); machineId++) {
		for (int taskPos = 0; taskPos < sol.machines()[machineId].countTasks(); taskPos++) {
			int taskId;
			taskId = sol.machines()[machineId].getTask(taskPos);

			assert(taskId >= 0);
			assert(taskId < sol.pbm().taskCount());

			ns << taskId;
			//			if (DEBUG) cout << "[DEBUG] operator<< " << taskId << endl;

			currentTask++;
			currentItem++;
		}
		ns << machineSeparator;
		//		if (DEBUG) cout << "[DEBUG] operator<< " << machineSeparator << endl;

		currentItem++;
	}

	//	if (DEBUG) cout << "[DEBUG] operator<< En total se mandaron " << currentItem << " integers." << endl;

	assert(currentTask == sol.pbm().taskCount());
	assert(currentItem == sol.pbm().taskCount() + sol.pbm().machineCount());

	return ns;
}

// ===================================
// Deserialización de la solución.
// ===================================
NetStream& operator >>(NetStream& ns, Solution& sol) {
	//	if (DEBUG) cout << endl << "[DEBUG] operator >>(NetStream& ns, Solution& sol)" << endl;

	int machineSeparator = -1;

	int currentTask = 0;
	int currentMachine = 0;

	//	if (DEBUG) cout << "[DEBUG] operator>> voy a leer "
	//			<< sol.pbm().taskCount() + sol.pbm().machineCount() <<
	//			" integers." << endl;

	//	if (DEBUG) cout << "[DEBUG] operator>> cantidad actual de tasks " << sol.countTasks() << " las voy a vaciar." << endl;
	sol.emptyTasks();

	for (int pos = 0; pos < sol.pbm().taskCount() + sol.pbm().machineCount(); pos++) {
		int currentValue;
		ns >> currentValue;

		//		if (DEBUG) cout << "[DEBUG] operator>> currentMachine:" << currentMachine
		//				<< " currentTask:" << currentTask << " currentValue:" << currentValue << endl;

		//		if (DEBUG) cout << "[DEBUG] operator>> " << currentValue << endl;

		if (currentValue == machineSeparator) {
			assert(currentMachine < sol.pbm().machineCount());
			currentMachine++;
		} else {
			assert(currentValue >= 0);
			assert(currentValue < sol.pbm().taskCount());
			assert(currentMachine >= 0);
			assert(currentMachine < sol.pbm().machineCount());

			sol.addTask(currentMachine, currentValue);
			currentTask++;
		}
	}

	//	if (DEBUG) cout << "[DEBUG] operator >> sol.pbm().taskCount() = " << sol.pbm().taskCount() << endl;
	//	if (DEBUG) cout << "[DEBUG] operator >> currentTask = " << currentTask << endl;

	assert(sol.machines().size() == sol.pbm().machineCount());
	assert(currentTask == sol.pbm().taskCount());

	sol.markAsInitialized();
	sol.validate();

	return ns;
}

Solution& Solution::operator=(const Solution &sol) {
	for (int machineId = 0; machineId < sol._machines.size(); machineId++) {
		_machines[machineId] = sol._machines[machineId];
	}

	_initialized = sol._initialized;

	return *this;
}

bool Solution::operator==(const Solution& sol) const {
	if (sol.pbm() != _pbm)
		return false;
	return true;
}

bool Solution::operator!=(const Solution& sol) const {
	return !(*this == sol);
}

bool Solution::isInitilized() const {
	return _initialized;
}

void Solution::emptyTasks() {
	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		_machines[machineId].emptyTasks();
	}
}

int Solution::countTasks() {
	int count = 0;
	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		count += _machines[machineId].countTasks();
	}
	return count;
}

// ===================================
// Inicializo la solución.
// ===================================
void Solution::initializeStaticMCT() {
	//if (DEBUG) cout << endl << "[DEBUG] Inicialización MCT Estática" << endl;

	initializeMCT(0, 1);
}

void Solution::initializeRandomMCT() {
	//if (DEBUG) cout << endl << "[DEBUG] Inicialización MCT Aleatoria" << endl;

	int startTask = rand_int(0, _pbm.taskCount() - 1);
	int direction = rand_int(0, 1);
	if (direction == 0)
		direction = -1;

	initializeMCT(startTask, direction);
}

void Solution::initializeMCT(int startTask, int direction) {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MCT" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	int currentTask;
	for (int taskOffset = 0; taskOffset < _pbm.taskCount(); taskOffset++) {
		currentTask = startTask + (direction * taskOffset);
		if (currentTask < 0)
			currentTask = _pbm.taskCount() + currentTask;
		currentTask = currentTask % _pbm.taskCount();

		double minFitness;
		minFitness = infinity();

		int minFitnessMachineId;
		minFitnessMachineId = -1;

		for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
			double makespan;
			makespan = (machineMakespan[machineId]
					+ _pbm.expectedTimeToCompute(currentTask, machineId));
			double auxFitness;
			auxFitness = _pbm.getMakespanWeight() * makespan;

			if (auxFitness < minFitness) {
				minFitness = auxFitness;
				minFitnessMachineId = machineId;
			}
		}

		machineMakespan[minFitnessMachineId] += _pbm.expectedTimeToCompute(
				currentTask, minFitnessMachineId);

		_machines[minFitnessMachineId].addTask(currentTask);
	}
}

void Solution::initializeMinMin() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	while (unmappedTasksCount > 0) {
		double minCT;
		minCT = infinity();

		int minCTTaskId;
		minCTTaskId = -1;

		int minCTMachineId;
		minCTMachineId = -1;

		for (int taskId = 0; taskId < _pbm.taskCount(); taskId++) {
			if (taskIsUnmapped[taskId]) {
				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					if ((machineMakespan[machineId]
							+ _pbm.expectedTimeToCompute(taskId, machineId))
							< minCT) {
						minCT = machineMakespan[machineId]
								+ _pbm.expectedTimeToCompute(taskId, machineId);
						minCTTaskId = taskId;
						minCTMachineId = machineId;
					}
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[minCTTaskId] = false;

		machineMakespan[minCTMachineId] += _pbm.expectedTimeToCompute(
				minCTTaskId, minCTMachineId);
		_machines[minCTMachineId].addTask(minCTTaskId);
	}
}

void Solution::initializeMinWRR5() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < _pbm.taskCount(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
					first_aux = machineMakespan[machineId]
							+ _pbm.expectedTimeToCompute(taskId, machineId);

					if (first_aux <= first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				if (machineMakespan[first_best_machineId] > 0) {
					if (_pbm.expectedTimeToCompute(taskId, first_best_machineId)
							== 0) {
						second_aux = machineMakespan[first_best_machineId]
								/ _pbm.taskPriority(taskId);
					} else {
						float rr;
						rr = (machineMakespan[first_best_machineId]
								+ _pbm.expectedTimeToCompute(taskId,
										first_best_machineId))
								/ _pbm.expectedTimeToCompute(taskId,
										first_best_machineId);
						second_aux = rr / _pbm.taskPriority(taskId);
					}
				} else {
					float aux;
					aux = _pbm.expectedTimeToCompute(taskId,
							first_best_machineId) / _pbm.taskPriority(taskId);
					second_aux = aux / aux + 1;
				}

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeMinWRR60() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < taskIsUnmapped.size(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					if (machineMakespan[machineId] > 0) {
						first_aux = (machineMakespan[machineId]
								/ _pbm.taskPriority(taskId))
								* (_machines[machineId].countTasks() + 1);
					} else {
						first_aux = 1 / _pbm.taskPriority(taskId);
					}

					if (first_aux < first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				second_aux = (machineMakespan[first_best_machineId]
						+ _pbm.expectedTimeToCompute(taskId,
								first_best_machineId)) / _pbm.taskPriority(
						taskId);

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeMinWRR61() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < taskIsUnmapped.size(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					if (_pbm.expectedTimeToCompute(taskId, machineId) == 0) {
						first_aux = 0.0;
					} else {
						if (machineMakespan[machineId] > 0) {
							float rr;
							rr = (machineMakespan[machineId]
									+ _pbm.expectedTimeToCompute(taskId,
											machineId))
									/ _pbm.expectedTimeToCompute(taskId,
											machineId);
							first_aux = rr / _pbm.taskPriority(taskId);
						} else {
							first_aux = 1 / _pbm.taskPriority(taskId);
						}
					}

					if (first_aux < first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				second_aux = machineMakespan[first_best_machineId]
						/ _pbm.taskPriority(taskId);

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeMinWRR62() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < taskIsUnmapped.size(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					if (machineMakespan[machineId] > 0) {
						first_aux = machineMakespan[machineId]
								/ _pbm.taskPriority(taskId);
					} else {
						first_aux = 1 / _pbm.taskPriority(taskId);
					}

					if (first_aux < first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				second_aux = _pbm.expectedTimeToCompute(taskId,
						first_best_machineId) / _pbm.taskPriority(taskId);

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeMinWRR0() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < taskIsUnmapped.size(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					first_aux = machineMakespan[machineId]
							+ _pbm.expectedTimeToCompute(taskId, machineId);
					if (first_aux > 0) {
						first_aux += (machineMakespan[machineId]
								/ (_pbm.taskPriority(taskId)
										* (_machines[machineId].countTasks()
												+ 1)));
					}

					if (first_aux < first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				second_aux = first_min;

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeMinWRR4() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización MIN-MIN" << endl;

	vector<double> machineMakespan;
	machineMakespan.reserve(_pbm.machineCount() + 1);

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++)
		machineMakespan.push_back(0.0);

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount() + 1);

	for (int taskId = 0; taskId < _pbm.taskCount(); taskId++)
		taskIsUnmapped.push_back(true);

	int unmappedTasksCount = _pbm.taskCount();

	float first_min;
	float first_aux;
	int first_best_machineId;

	float second_min;
	float second_aux;
	int second_best_machineId;
	int second_best_taskId;

	while (unmappedTasksCount > 0) {
		second_best_taskId = -1;
		second_best_machineId = -1;
		second_min = FLT_MAX;

		for (int taskId = 0; taskId < taskIsUnmapped.size(); taskId++) {
			first_min = FLT_MAX;
			second_aux = FLT_MAX;
			first_best_machineId = -1;

			if (taskIsUnmapped[taskId]) {
				first_aux = 0.0;

				for (int machineId = 0; machineId < machineMakespan.size(); machineId++) {
					if (machineMakespan[machineId] > 0) {
						float rr;
						rr
								= (machineMakespan[machineId]
										+ _pbm.expectedTimeToCompute(taskId,
												machineId))
										/ _pbm.expectedTimeToCompute(taskId,
												machineId);
						first_aux = rr / _pbm.taskPriority(taskId);
					} else {
						float aux;
						aux = _pbm.expectedTimeToCompute(taskId, machineId)
								/ _pbm.taskPriority(taskId);
						first_aux = aux / aux + 1;
					}

					if (first_aux < first_min) {
						first_min = first_aux;
						first_best_machineId = machineId;
					}
				}

				second_aux = machineMakespan[first_best_machineId]
						+ _pbm.expectedTimeToCompute(taskId,
								first_best_machineId);

				if (second_aux <= second_min) {
					second_min = second_aux;

					second_best_taskId = taskId;
					second_best_machineId = first_best_machineId;
				}
			}
		}

		unmappedTasksCount--;
		taskIsUnmapped[second_best_taskId] = false;
		machineMakespan[second_best_machineId] += _pbm.expectedTimeToCompute(
				second_best_taskId, second_best_machineId);

		_machines[second_best_machineId].addTask(second_best_taskId);
	}
}

void Solution::initializeRandom() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización random" << endl;

	int startTask = rand_int(0, _pbm.taskCount() - 1);
	int direction = rand_int(0, 1);
	if (direction == 0)
		direction = -1;

	int currentTask;
	for (int taskOffset = 0; taskOffset < _pbm.taskCount(); taskOffset++) {
		currentTask = startTask + (direction * taskOffset);
		if (currentTask < 0)
			currentTask = _pbm.taskCount() + currentTask;
		currentTask = currentTask % _pbm.taskCount();

		int currentMachine;
		currentMachine = rand_int(0, _pbm.machineCount() - 1);

		_machines[currentMachine].addTask(currentTask);
	}
}

void Solution::initializeSufferage() {
	//	if (DEBUG) cout << endl << "[DEBUG] Inicialización Sufferage" << endl;
	int unmappedTasks = _pbm.taskCount();

	vector<bool> taskIsUnmapped;
	taskIsUnmapped.reserve(_pbm.taskCount());
	for (int i = 0; i < _pbm.taskCount(); i++) {
		taskIsUnmapped[i] = true;
	}

	vector<double> machinesMakespan;
	machinesMakespan.reserve(_pbm.machineCount());
	for (int i = 0; i < _pbm.machineCount(); i++) {
		machinesMakespan[i] = 0.0;
	}

	int maxSufferageTaskId;
	int maxSufferageMachineId;
	double maxSufferageValue;

	while (unmappedTasks > 0) {
		maxSufferageTaskId = -1;
		maxSufferageMachineId = -1;
		maxSufferageValue = 0.0;

		for (int taskId = 0; taskId < _pbm.taskCount(); taskId++) {
			if (taskIsUnmapped[taskId]) {
				int minMakespanMachineId;
				minMakespanMachineId = -1;

				int secondMinMakespanMachineId;
				secondMinMakespanMachineId = -1;

				assert(_pbm.machineCount() > 2);

				for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
					double currentMakespan;
					double minMakespan;
					double secondMinMakespan;

					currentMakespan = machinesMakespan[machineId]
							+ _pbm.expectedTimeToCompute(taskId, machineId);

					if (minMakespanMachineId != -1) {
						minMakespan = machinesMakespan[minMakespanMachineId]
								+ _pbm.expectedTimeToCompute(taskId,
										minMakespanMachineId);
					}
					if (secondMinMakespanMachineId != -1) {
						secondMinMakespan
								= machinesMakespan[secondMinMakespanMachineId]
										+ _pbm.expectedTimeToCompute(taskId,
												secondMinMakespanMachineId);
					}
					if (minMakespanMachineId == -1) {
						minMakespanMachineId = machineId;
					} else if (minMakespan > currentMakespan) {
						secondMinMakespanMachineId = minMakespanMachineId;
						minMakespanMachineId = machineId;
					} else if (secondMinMakespanMachineId == -1) {
						secondMinMakespanMachineId = machineId;
					} else if (secondMinMakespan > currentMakespan) {
						secondMinMakespanMachineId = machineId;
					}
				}

				double minMakespan;
				double secondMinMakespan;
				double sufferageValue;

				minMakespan = machinesMakespan[minMakespanMachineId]
						+ _pbm.expectedTimeToCompute(taskId,
								minMakespanMachineId);

				secondMinMakespan
						= machinesMakespan[secondMinMakespanMachineId]
								+ _pbm.expectedTimeToCompute(taskId,
										secondMinMakespanMachineId);

				sufferageValue = secondMinMakespan - minMakespan;

				if ((maxSufferageMachineId == -1) || (maxSufferageTaskId == -1)) {
					maxSufferageTaskId = taskId;
					maxSufferageMachineId = minMakespanMachineId;
					maxSufferageValue = sufferageValue;
				} else if (maxSufferageValue < sufferageValue) {
					maxSufferageTaskId = taskId;
					maxSufferageMachineId = minMakespanMachineId;
					maxSufferageValue = sufferageValue;
				}
			}
		}

		assert((maxSufferageTaskId >= 0) && (maxSufferageMachineId >= 0));

		machinesMakespan[maxSufferageMachineId]
				= machinesMakespan[maxSufferageMachineId]
						+ _pbm.expectedTimeToCompute(maxSufferageTaskId,
								maxSufferageMachineId);

		_machines[maxSufferageMachineId].addTask(maxSufferageTaskId);

		taskIsUnmapped[maxSufferageTaskId] = false;
		unmappedTasks--;
	}
}

void Solution::markAsInitialized() {
	_initialized = true;
}

void Solution::initialize(int mypid, int pnumber, const int solutionIndex) {
	if (DEBUG) {
		cout << "[DEBUG] Solution::initialize" << endl;
		cout << "pypid: " << mypid << endl;
		cout << "pnumber: " << pnumber << endl;
		cout << "solutionIndex: " << pnumber << endl;
	}

	markAsInitialized();

	if (solutionIndex == 0) {
		// Inicialización usando una versión determinista de la heurística MCT.
		// La solución 0 (cero) es idéntica en todos las instancias de ejecución.
		// Utilizo la solución 0 (cero) como referencia de mejora del algoritmo.

		//initializeStaticMCT();
		initializeMinMin();

		//NOTE: NO EVALUAR FITNESS ANTES DE ESTA ASIGNACIÓN!!!
		Solution::_awrr_reference = accumulatedWeightedResponseRatio();
		Solution::_makespan_reference = makespan();

		if (mypid == 0) {
			cout << "MCT reference fitness: " << fitness();
			cout << ", WRR: " << accumulatedWeightedResponseRatio();
			cout << ", Makespan: " << makespan() << endl << endl;
		}	
	} else {
		initializeRandomMCT();
		if (DEBUG) {
			cout << endl << "[proc " << mypid << "] ";
			cout << "Random MCT fitness: " << fitness();
			cout << ", WRR: " << accumulatedWeightedResponseRatio();
			cout << ", Makespan: " << makespan() << endl;
		}
	}
	if (DEBUG) {
		cout << "[DEBUG] Init ready on pid: " << mypid << endl;
	}
}

//void Solution::initialize(int mypid, int pnumber, const int solutionIndex) {
//	markAsInitialized();
//
//	if (solutionIndex == 0) {
//		// Inicialización usando una versión determinista de la heurística MCT.
//		// La solución 0 (cero) es idéntica en todos las instancias de ejecución.
//		// Utilizo la solución 0 (cero) como referencia de mejora del algoritmo.
//
//		initializeStaticMCT();
//
//		//NOTE: NO EVALUAR FITNESS ANTES DE ESTA ASIGNACIÓN!!!
//		Solution::_awrr_reference = accumulatedWeightedResponseRatio();
//		Solution::_makespan_reference = makespan();
//		if (DEBUG) {
//			cout << endl << "[proc " << mypid << "] ";
//			cout << "MCT reference fitness: " << fitness();
//			cout << ", WRR: " << accumulatedWeightedResponseRatio();
//			cout << ", Makespan: " << makespan() << endl;
//		}
//	} else {
//		int proceso_actual = mypid;
//		int offset_heuristica_actual = solutionIndex - 1;
//
//		if (offset_heuristica_actual == 0) {
//			// Inicialización usando una heurística "pesada": MIN-MIN.
//			// Utilizo MIN-MIN para un único elemento de la población inicial.
//
//			initializeMinMin();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "Min-Min fitness: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//
//			showCustomStatics();
//		} else if (offset_heuristica_actual == 1) {
//			initializeMinWRR0();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR0: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 2) {
//			// Inicialización usando otra heurística "pesada" diferente: Sufferage.
//			// Utilizo Sufferage para un único elemento de la población inicial.
//
//			initializeSufferage();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "Sufferage fitness: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 3) {
//			initializeMinWRR4();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR4: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 4) {
//			// Inicialización usando otra heurística "pesada" diferente: Sufferage.
//			initializeMinWRR5();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR5: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 5) {
//			initializeMinWRR60();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR60: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 6) {
//			initializeMinWRR61();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR61: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//		} else if (offset_heuristica_actual == 7) {
//			initializeMinWRR62();
//			if (DEBUG) {
//				cout << endl << "[proc " << proceso_actual << "] ";
//				cout << "MinMinWRR62: " << fitness();
//				cout << ", WRR: " << accumulatedWeightedResponseRatio();
//				cout << ", Makespan: " << makespan() << endl;
//			}
//
//			exit(-1);
//		}
//	}
//}

bool Solution::validate() const {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::validate" << endl;
	if (true) {
		for (int t = 0; t < _pbm.taskCount(); t++) {
			int machineId, taskPos;
			//assert(findTask(t, machineId, taskPos));
		}

		if (_machines.size() == _pbm.machineCount()) {
			int taskCount = 0;

			for (int machineId = 0; machineId < _machines.size(); machineId++) {
				for (int taskPos = 0; taskPos
						< _machines[machineId].countTasks(); taskPos++) {
					taskCount++;

					int taskId;
					taskId = _machines[machineId].getTask(taskPos);

					assert(_machines[machineId].hasTask(taskId));

					if ((taskId < 0) || (taskId >= _pbm.taskCount())) {
						if (DEBUG)
							cout << endl
									<< "[DEBUG] (taskId < 0) || (taskId >= _pbm.taskCount())"
									<< endl;
						assert(false);
					}
				}
			}

			if (taskCount != _pbm.taskCount()) {
				if (DEBUG)
					cout << endl << "[DEBUG] taskCount != _pbm.taskCount()"
							<< endl;
				cout << endl << "[DEBUG] taskCount:" << taskCount
						<< " _pbm.taskCount():" << _pbm.taskCount() << endl;
				assert(false);
			}
		} else {
			if (DEBUG)
				cout << endl
						<< "[DEBUG] this->_machines.size() == _pbm.machineCount()"
						<< endl;
			assert(false);
		}
	} else {
		assert(false);
	}

	return true;
}

void Solution::show(ostream &output) {
	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
		for (int taskPos = 0; taskPos < _machines[machineId].countTasks(); taskPos++) {
			int taskId;
			taskId = _machines[machineId].getTask(taskPos);

			output << taskId << endl;
		}
		output << "-1" << endl;
	}
}

void Solution::showCustomStatics() {
	cout << endl << "[= Statics RR ==================]" << endl;

	// Tiempo de respuesta promedio por prioridad
	int total_count = 0;
	double total_rr_sum = 0.0;
	double total_rr_worst = 0.0;

	cout << " * Avg. response ratio by priority." << endl;
	for (int priority = 0; priority <= 10; priority++) {
		cout << "   priority = " << priority;

		int count = 0;
		double rr_aux = 0.0;
		double rr_sum = 0.0;
		double rr_worst = 0.0;

		for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
			double partial_cost;
			partial_cost = 0.0;

			for (int taskPos = 0; taskPos < _machines[machineId].countTasks(); taskPos++) {
				int taskId;
				taskId = _machines[machineId].getTask(taskPos);

				if (_pbm.taskPriority(taskId) == priority) {
					count++;

					if (_pbm.expectedTimeToCompute(taskId, machineId) == 0.0) {
						rr_aux = 0.0;
					} else {
						rr_aux = (partial_cost + _pbm.expectedTimeToCompute(
								taskId, machineId))
								/ _pbm.expectedTimeToCompute(taskId, machineId);
					}
					rr_sum += rr_aux;

					if (rr_worst <= rr_aux) {
						rr_worst = rr_aux;
					}
				}

				partial_cost += _pbm.expectedTimeToCompute(taskId, machineId);
			}
		}

		total_count += count;
		total_rr_sum += rr_sum;

		if (total_rr_worst <= rr_worst) {
			total_rr_worst = rr_worst;
		}

		if (count > 0) {
			cout << " (" << count << " tasks)";
			cout << " >> avg. rr = " << rr_sum / count;
			cout << ", worst rr = " << rr_worst << endl;
			rr_sum = 0.0;
			count = 0;
		} else {
			cout << " N/A" << endl;
		}
	}

	// Tiempo de respuesta promedio
	cout << " * Avg. response ratio: " << total_rr_sum / total_count << endl;

	// Peor tiempo de respuesta
	cout << " * Worst response ratio: " << total_rr_worst << endl;

	cout << endl << "[= Statics Makespan ============]" << endl;

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
		cout << "Machine " << machineId << ": "
				<< _machines[machineId].getMakespan() << endl;
	}

	cout << "[===============================]" << endl;
}

// ===================================
// Fitness de la solución.
// ===================================
double Solution::fitness() {
	assert(_initialized);

	double maxMakespan = 0.0;
	double awrr = 0.0;

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
		awrr += _machines[machineId].getAccumulatedWeightedResponseRatio();

		if (_machines[machineId].getMakespan() > maxMakespan) {
			maxMakespan = _machines[machineId].getMakespan();
		}
	}

	double normalized_awrr;
	if (awrr > 0) {
		normalized_awrr = (awrr + Solution::_awrr_reference)
				/ Solution::_awrr_reference;
	} else {
		normalized_awrr = 0;
	}

	double normalized_makespan;
	normalized_makespan = (maxMakespan + Solution::_makespan_reference)
			/ Solution::_makespan_reference;

	//	cout << "Norm mks: " << normalized_makespan << ", norm wrr: " << normalized_awrr << endl;
	//	cout << "Peso mks: " << _pbm.getMakespanWeight() << ", peso wrr: " << _pbm.getWRRWeight() << endl;

	double fitness;
	fitness = (_pbm.getMakespanWeight() * normalized_makespan)
			+ (_pbm.getWRRWeight() * normalized_awrr);

	assert(!(fitness == INFINITY));
	assert(!(fitness == NAN));

	return fitness;
}

double Solution::makespan() {
	if (!_initialized) {
		return infinity();
	}

	double maxMakespan = 0.0;

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
		if (_machines[machineId].getMakespan() > maxMakespan) {
			maxMakespan = _machines[machineId].getMakespan();
		}
	}

	return maxMakespan;
}

double Solution::accumulatedWeightedResponseRatio() {
	if (!_initialized) {
		return infinity();
	}

	double awrr = 0.0;

	for (int machineId = 0; machineId < _pbm.machineCount(); machineId++) {
		awrr = awrr
				+ _machines[machineId].getAccumulatedWeightedResponseRatio();

		//		if (DEBUG) {
		//			cout << "[INFO] machine: " << machineId << " awrr:" << _machines[machineId].getAccumulatedWeightedResponseRatio() << endl;
		//		}
	}

	return awrr;
}

int Solution::length() const {
	return _pbm.taskCount();
}

unsigned int Solution::size() const {
	return (_pbm.taskCount() * sizeof(int)) + (_pbm.machineCount()
			* sizeof(int)) + sizeof(int);
}

int Solution::distanceTo(const Solution& solution) const {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::distanceTo start" << endl;
	int distance = 0;

	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		for (int taskPos = 0; taskPos < _machines[machineId].countTasks(); taskPos++) {
			int taskId;
			taskId = _machines[machineId].getTask(taskPos);

			if (solution._machines[machineId].countTasks() > taskPos) {
				if (solution._machines[machineId].getTask(taskPos) == taskId) {
					// La tarea actual es ejecutada en la misma máquina y en la misma
					// posición en ambas soluciones.
				} else {
					distance++;
				}
			} else {
				distance++;
			}
		}
	}
	return distance;
}

bool Solution::findTask(const int taskId, int& foundMachineId,
		int& foundTaskPos) {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::findTask" << endl;
	foundMachineId = -1;
	foundTaskPos = -1;

	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		if (_machines[machineId].hasTask(taskId)) {
			for (int taskPos = 0; taskPos < _machines[machineId].countTasks(); taskPos++) {
				if (_machines[machineId].getTask(taskPos) == taskId) {
					foundMachineId = machineId;
					foundTaskPos = taskPos;

					return true;
				}
			}

			assert(false);
		}
	}

	assert(false);
}

double Solution::getMachineFitness(int machineId) {
	double awrr_ratio = (accumulatedWeightedResponseRatio()
			+ Solution::_awrr_reference) / Solution::_awrr_reference;
	double makespan_ratio = (makespan() + Solution::_makespan_reference)
			/ Solution::_makespan_reference;
	return (_pbm.getWRRWeight() * awrr_ratio) + (_pbm.getMakespanWeight()
			* makespan_ratio);
}

void Solution::doLocalSearch() {
	//	if (DEBUG)
	//		cout << endl << "[DEBUG] Solution::doLocalSearch begin" << endl;

	vector<double> fitnessByMachine;

	for (unsigned int machineId = 0; machineId < this->machines().size(); machineId++) {
		fitnessByMachine.push_back(getMachineFitness(machineId));
	}

	RouletteWheel roulette(fitnessByMachine, true);

	vector<int> maquinasSeleccionadas;
	for (int i = 0; i < PALS_MAQ; i++) {
		maquinasSeleccionadas.push_back(roulette.drawOneByIndex());
	}

	double fitnessInicial = this->fitness();
	bool solucionAceptada = false;

	for (unsigned int machinePos = 0; (machinePos
			< maquinasSeleccionadas.size()) && !solucionAceptada; machinePos++) {

		int machineId;
		machineId = maquinasSeleccionadas[machinePos];

		// PALS aleatorio para HCSP.
		//		if (DEBUG) cout << endl << "[DEBUG] Búsqueda en la máquina " << machineId << endl;

		bool finBusqMaquina;
		finBusqMaquina = false;

		for (int intento = 0; (intento < PALS_MAX_INTENTOS) && !finBusqMaquina; intento++) {
			double mejorMovimientoFitness;
			int mejorMovimientoTaskPos, mejorMovimientoDestinoTaskPos,
					mejorMovimientoDestinoMachineId;
			mejorMovimientoFitness = fitnessInicial;
			mejorMovimientoTaskPos = -1;
			mejorMovimientoDestinoTaskPos = -1;
			mejorMovimientoDestinoMachineId = -1;

			//			if (DEBUG) cout << endl << "[DEBUG] Intento " << intento << endl;

			// Itero en las tareas de la máquina actual.
			int startTaskOffset, endTaskOffset;
			if (this->machines()[machineId].countTasks() > PALS_TOP_M) {
				// Si la cantidad de tareas en la máquina actual es mayor que PALS_TOP_M.
				double rand;
				rand = rand01();

				double aux;
				aux = rand * this->machines()[machineId].countTasks();

				startTaskOffset = (int) aux;
				endTaskOffset = startTaskOffset + PALS_TOP_M;
			} else {
				// Si hay menos o igual cantidad de tareas en la máquina actual que el
				// tope PALS_TOP_M, las recorro todas.
				startTaskOffset = 0;
				endTaskOffset = this->machines()[machineId].countTasks();
			}

			//			if (DEBUG) cout << endl << "[DEBUG] En la máquina actual hay " << this->machines()[machineId].countTasks()
			//					<< " tareas, pruebo desde la " << startTaskOffset << " a la " << endTaskOffset << endl;

			for (int taskOffset = startTaskOffset; taskOffset < endTaskOffset; taskOffset++) {
				int taskPos;
				taskPos = taskOffset % this->machines()[machineId].countTasks();

				int taskId;
				taskId = this->machines()[machineId].getTask(taskPos);

				// Itero en las tareas de las otras máquinas.
				int startSwapTaskOffset, countSwapTaskOffset;

				if ((this->pbm().taskCount() - 1) > PALS_TOP_T) {
					// Si la cantidad de las tareas del problema menos la tarea que estoy
					// intentando mover es mayor que PALS_TOP_T.
					double rand;
					rand = rand01();

					double aux;
					aux = rand * this->pbm().taskCount();

					startSwapTaskOffset = (int) aux;
					countSwapTaskOffset = PALS_TOP_T;
				} else {
					// Si hay menos o igual cantidad de tareas en el problema que el número
					// PALS_TOP_T las recorro todas menos la que estoy intentando mover.
					startSwapTaskOffset = 0;
					countSwapTaskOffset = this->pbm().taskCount() - 1;
				}

				double movimientoFitness;
				movimientoFitness = 0.0;

				//				if (DEBUG) cout << endl << "[DEBUG] En el problema hay " << this->pbm().taskCount()
				//						<< " tareas, pruebo desde la " << startSwapTaskOffset << endl;

				for (int swapTaskOffset = startSwapTaskOffset; countSwapTaskOffset
						> 0; swapTaskOffset++) {
					assert(swapTaskOffset < (2*this->pbm().taskCount()));

					int swapTaskId;
					swapTaskId = swapTaskOffset % this->pbm().taskCount();

					//					if (DEBUG) cout << endl << "[DEBUG] Intento swapear taskId=" << taskId
					//							<< "con taskId=" << swapTaskId << endl;

					if (swapTaskId != taskId) {
						countSwapTaskOffset--;

						int swapMachineId, swapTaskPos;
						assert(this->findTask(swapTaskId, swapMachineId, swapTaskPos));

						//==============================================================
						//TODO: Optimizar!!!
						//==============================================================
						this->swapTasks(machineId, taskPos, swapMachineId,
								swapTaskPos);
						movimientoFitness = this->fitness();
						this->swapTasks(swapMachineId, swapTaskPos, machineId,
								taskPos);
						//==============================================================

						if (movimientoFitness < mejorMovimientoFitness) {
							//							cout << endl << "Mejora parcial " << movimientoFitness - mejorMovimientoFitness << endl;

							mejorMovimientoFitness = movimientoFitness;
							mejorMovimientoTaskPos = taskPos;
							mejorMovimientoDestinoMachineId = swapMachineId;
							mejorMovimientoDestinoTaskPos = swapTaskPos;
						}
					}
				}
			}

			if (mejorMovimientoFitness < fitnessInicial) {
				//				if (DEBUG) cout << endl << "[DEBUG] Se mejoró la solución!" << endl;
				this->swapTasks(machineId, mejorMovimientoTaskPos,
						mejorMovimientoDestinoMachineId,
						mejorMovimientoDestinoTaskPos);
				finBusqMaquina = true;
			}
		}

		solucionAceptada = (this->fitness() / fitnessInicial)
				>= PALS_UMBRAL_MEJORA;
	}
}

void Solution::mutate() {
	//if (DEBUG)	cout << endl << "[DEBUG] Solution::mutate" << endl;

	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		if (rand01() <= MUT_MAQ) {
			if (_machines[machineId].countTasks() == 0) {
				// Cada máquina sin tareas se le asigna la tarea que
				// mejor puede ejecutar.
				{
					int bestTaskIdForMachine;
					bestTaskIdForMachine = _pbm.getBestTaskIdForMachine(
							machineId);

					int origenMachineId, origenTaskPos;
					assert(findTask(bestTaskIdForMachine, origenMachineId, origenTaskPos));

					if (_machines[origenMachineId].countTasks() > 1) {
						if (_pbm.getBestTaskIdForMachine(origenMachineId)
								!= bestTaskIdForMachine) {
							_machines[origenMachineId].removeTask(origenTaskPos);
							_machines[machineId].addTask(bestTaskIdForMachine);
						}
					}
				}

				if (_machines[machineId].countTasks() == 0) {
					int mostLoadedMachineId = getMaxCostMachineId();

					int bestTaskPosForMachine;
					bestTaskPosForMachine
							= getMinDestinationCostTaskPosByMachine(
									mostLoadedMachineId, machineId);

					int bestTaskIdForMachine;
					bestTaskIdForMachine
							= _machines[mostLoadedMachineId].getTask(
									bestTaskPosForMachine);

					_machines[mostLoadedMachineId].removeTask(
							bestTaskPosForMachine);
					_machines[machineId].addTask(bestTaskIdForMachine);
				}
			} else if (_machines[machineId].countTasks() > 0) {
				for (int selectedTaskPos = 0; selectedTaskPos
						< _machines[machineId].countTasks(); selectedTaskPos++) {

					if (rand01() < MUT_TASK) {
						int neighbourhood;
						neighbourhood = rand_int(0, 3);

						if (neighbourhood == 0) {
							// Se intercambia con la tarea que mejor puede ejecutarse en la máquina actual de
							// la máquina en la que mejor puede ejecutarse.

							// Obtengo la máquina que que mejor puede ejecutar la tarea.
							int selectedTaskId;
							selectedTaskId = _machines[machineId].getTask(
									selectedTaskPos);

							int bestMachineId;
							bestMachineId = _pbm.getBestMachineForTaskId(
									selectedTaskId);

							if (bestMachineId != machineId) {
								if (_machines[bestMachineId].countTasks() > 0) {
									// Si la máquina destino tiene al menos una tarea, obtengo la tarea
									// con menor costo de ejecución en la máquina sorteada.
									int minCostTaskPosOnMachine;
									minCostTaskPosOnMachine
											= getMinDestinationCostTaskPosByMachine(
													bestMachineId, machineId);

									// Hago un swap entre las tareas de las máquinas.
									swapTasks(machineId, selectedTaskPos,
											bestMachineId,
											minCostTaskPosOnMachine);
								} else {
									_machines[bestMachineId].addTask(
											selectedTaskId);
									_machines[machineId].removeTask(
											selectedTaskPos);
								}
							}
						}

						if (neighbourhood == 1) {
							// Se intercambia con la tarea de la máquina con menor makespan que puede ejecutarse
							// más eficientemente en la máquina actual.

							// Obtengo la máquina que aporta un menor costo al total de la solución.
							int minCostMachineId;
							minCostMachineId = getMinCostMachineId();

							int selectedTaskId;
							selectedTaskId = _machines[machineId].getTask(
									selectedTaskPos);

							if (_machines[minCostMachineId].countTasks() > 0) {
								// Si la máquina destino tiene al menos una tarea, obtengo la tarea
								// con menor costo de ejecución en la máquina sorteada.
								int minCostTaskPosOnMachine;
								minCostTaskPosOnMachine
										= getMinDestinationCostTaskPosByMachine(
												minCostMachineId, machineId);

								// Hago un swap entre las tareas de las máquinas.
								swapTasks(machineId, selectedTaskPos,
										minCostMachineId,
										minCostTaskPosOnMachine);
							} else {
								_machines[minCostMachineId].addTask(
										selectedTaskId);
								_machines[machineId].removeTask(selectedTaskPos);
							}
						}

						if (neighbourhood == 2) {
							// Se adelanta la posición en la cola de la tarea siempre y cuando la tarea
							// que le preceda tenga menor prioridad.
							for (int taskPos = selectedTaskPos; taskPos >= 1; taskPos--) {
								int taskId;
								taskId = _machines[machineId].getTask(taskPos);

								int anteriorTaskId;
								anteriorTaskId = _machines[machineId].getTask(
										taskPos - 1);

								if (_pbm.expectedTimeToCompute(taskId,
										machineId)
										< _pbm.expectedTimeToCompute(
												anteriorTaskId, machineId)) {
									_machines[machineId].swapTasks(taskPos,
											taskPos - 1);
								}
							}
						}

						if (neighbourhood == 3) {
							int minAWRRMachineId = getMinAWRRMachine();
							if (minAWRRMachineId != machineId) {
								int selectedTaskId;
								selectedTaskId = _machines[machineId].getTask(
										selectedTaskPos);

								_machines[minAWRRMachineId].addTask(
										selectedTaskId);
								_machines[machineId].removeTask(selectedTaskPos);

								// Se selecciona una tarea T según su función de PRIORIDAD y se
								// adelanta si lugar en la cola de ejecución.
								for (int
										taskPos =
												_machines[minAWRRMachineId].countTasks()
														- 1; taskPos >= 1; taskPos--) {
									int taskId;
									taskId
											= _machines[minAWRRMachineId].getTask(
													taskPos);

									int anteriorTaskId;
									anteriorTaskId
											= _machines[minAWRRMachineId].getTask(
													taskPos - 1);

									if (_pbm.expectedTimeToCompute(taskId,
											machineId)
											< _pbm.expectedTimeToCompute(
													anteriorTaskId, machineId)) {
										_machines[minAWRRMachineId].swapTasks(
												taskPos, taskPos - 1);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void Solution::addTask(const int machineId, const int taskId) {
	_machines[machineId].addTask(taskId);
}

void Solution::swapTasks(int machineId1, int taskPos1, int machineId2,
		int taskPos2) {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::swapTasks start" << endl;

	if (machineId1 != machineId2) {
		int taskId1 = machines()[machineId1].getTask(taskPos1);
		int taskId2 = machines()[machineId2].getTask(taskPos2);

		if (taskId1 != taskId2) {
			_machines[machineId1].setTask(taskId2, taskPos1);
			_machines[machineId2].setTask(taskId1, taskPos2);
		}
	} else {
		_machines[machineId1].swapTasks(taskPos1, taskPos2);
	}
}

void Solution::swapTasks(Solution& solution, const int taskId) {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::swapTasks" << endl;

	Solution& sol1 = *this;
	Solution& sol2 = solution;
	int machine1, machine2, taskPos1, taskPos2;

	assert(sol1.findTask(taskId, machine1, taskPos1));
	assert(sol2.findTask(taskId, machine2, taskPos2));

	if ((machine1 != machine2) || (taskPos1 != taskPos2)) {
		// Modifico la solución 1.
		// Borro la tarea de la ubicación original.
		sol1._machines[machine1].removeTask(taskPos1);

		// Inserto la tarea en la nueva ubicación.
		if (taskPos2 < sol1._machines[machine2].countTasks()) {
			sol1._machines[machine2].insertTask(taskId, taskPos2);
		} else {
			sol1._machines[machine2].addTask(taskId);
		}

		// Modifico la solución 2.
		// Borro la tarea de la ubicación original.
		sol2._machines[machine2].removeTask(taskPos2);

		// Inserto la tarea en la nueva ubicación.
		if (taskPos1 < sol2._machines[machine1].countTasks()) {
			sol2._machines[machine1].insertTask(taskId, taskPos1);
		} else {
			sol2._machines[machine1].addTask(taskId);
		}
	}
}

bool Solution::equalTasks(Solution& solution, const int taskId) {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::equalTasks" << endl;
	int machine1, machine2, taskPos1, taskPos2;

	assert(findTask(taskId, machine1, taskPos1));
	assert(solution.findTask(taskId, machine2, taskPos2));

	return (machine1 == machine2) && (taskPos1 == taskPos2);
}

char *Solution::to_String() const {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::to_String begin" << endl;
	int machineSeparator = -1;
	int endMark = -2;

	int rawPos = 0;
	char *rawChar = new char[size()];
	int *raw = (int*) rawChar;

	for (int machineId = 0; machineId < _machines.size(); machineId++) {
		for (int taskPos = 0; taskPos < _machines[machineId].countTasks(); taskPos++) {
			int taskId;
			taskId = _machines[machineId].getTask(taskPos);

			raw[rawPos] = taskId;
			rawPos += 1;
		}
		raw[rawPos] = machineSeparator;
		rawPos += 1;
	}
	raw[rawPos] = endMark;
	rawPos += 1;

	//	if (DEBUG) cout << endl << "[DEBUG] Solution::to_String end" << endl;

	return rawChar;
}

void Solution::to_Solution(char *_string_) {
	//	if (DEBUG) cout << endl << "[DEBUG] Solution::to_Solution" << endl;

	int *raw = (int*) _string_;

	int machineSeparator = -1;
	int endMark = -2;

	bool endFound = false;

	int currentTask = 0;
	int currentMachine = 0;

	for (int pos = 0; pos < (_pbm.taskCount() + _pbm.machineCount() + 1)
			&& !endFound; pos++) {

		int currentValue;
		currentValue = raw[pos];

		if (currentValue == endMark) {
			endFound = true;
		} else if (currentValue == machineSeparator) {
			assert(currentMachine < _pbm.machineCount());

			currentMachine++;
		} else {
			assert(currentValue >= 0);
			assert(currentValue < _pbm.taskCount());
			assert(currentMachine < _pbm.machineCount());

			_machines[currentMachine].addTask(currentValue);
			currentTask++;
		}
	}

	assert(_machines.size() == _pbm.machineCount());
	assert(currentTask == _pbm.taskCount());
	assert(endFound);

	markAsInitialized();
}

const vector<struct SolutionMachine>& Solution::machines() const {
	return _machines;
}

vector<struct SolutionMachine>& Solution::getMachines() {
	return _machines;
}

int Solution::getMinAWRRMachine() {
	int minAWRRMachineId = 0;
	double minAWRRValue = infinity();

	for (int machineId = 1; machineId < _machines.size(); machineId++) {
		double aux;
		aux = _machines[machineId].getAccumulatedWeightedResponseRatio();

		if (aux <= minAWRRValue) {
			minAWRRMachineId = machineId;
			minAWRRValue = aux;
		}
	}

	return minAWRRMachineId;
}

int Solution::getBestFitnessMachineId() {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::getBestFitnessMachineId" << endl;

	int bestFitnessMachineId = 0;
	double bestFitnessMachineValue = getMachineFitness(0);

	for (int machineId = 1; machineId < machines().size(); machineId++) {
		double currentMachineFitness;
		currentMachineFitness = getMachineFitness(machineId);

		if ((bestFitnessMachineValue > currentMachineFitness)
				&& (_pbm.direction() == minimize)) {

			bestFitnessMachineValue = currentMachineFitness;
			bestFitnessMachineId = machineId;

		} else if ((bestFitnessMachineValue < currentMachineFitness)
				&& (_pbm.direction() == maximize)) {

			bestFitnessMachineValue = currentMachineFitness;
			bestFitnessMachineId = machineId;
		}
	}

	return bestFitnessMachineId;
}

int Solution::getMinCostMachineId() {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::getMinCostMachineId" << endl;
	int minCostMachineId = 0;
	double minCostMachineValue = _machines[0].getMakespan();

	for (int machineId = 1; machineId < machines().size(); machineId++) {
		double currentMachineCost;
		currentMachineCost = _machines[machineId].getMakespan();

		if (minCostMachineValue > currentMachineCost) {
			minCostMachineValue = currentMachineCost;
			minCostMachineId = machineId;
		}
	}

	return minCostMachineId;
}

int Solution::getMaxCostMachineId() {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::getMaxCostMachineId" << endl;
	int maxCostMachineId = 0;
	double maxCostMachineValue = _machines[0].getMakespan();

	for (int machineId = 1; machineId < machines().size(); machineId++) {
		double currentMachineCost;
		currentMachineCost = _machines[machineId].getMakespan();

		if (maxCostMachineValue < currentMachineCost) {
			maxCostMachineValue = currentMachineCost;
			maxCostMachineId = machineId;
		}
	}

	return maxCostMachineId;
}

int Solution::getHighestPriorityTaskPosByMachine(int machineId) const {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::getHighestPriorityTaskPosByMachine" << endl;

	if (machines()[machineId].countTasks() > 0) {
		int highestPriorityTaskPos = 0;
		int highestPriorityTaskValue = _pbm.taskPriority(
				machines()[machineId].getTask(0));

		for (int taskPos = 1; taskPos < machines()[machineId].countTasks(); taskPos++) {
			int currentTaskPriority;
			currentTaskPriority = _pbm.taskPriority(
					machines()[machineId].getTask(taskPos));

			if (highestPriorityTaskValue > currentTaskPriority) {
				highestPriorityTaskValue = currentTaskPriority;
				highestPriorityTaskPos = taskPos;
			}
		}

		return highestPriorityTaskPos;
	} else {
		return -1;
	}
}

int Solution::getMinCostTaskPosByMachine(int machineId) const {
	// if (DEBUG) cout << endl << "[DEBUG] Solution::getMinCostTaskPosByMachine" << endl;
	assert(machines()[machineId].countTasks() > 0);

	int minCostTaskPos = 0;
	double minCostTaskValue = _pbm.expectedTimeToCompute(
			machines()[machineId].getTask(0), machineId);

	for (int taskPos = 1; taskPos < machines()[machineId].countTasks(); taskPos++) {
		double currentTaskCost;
		currentTaskCost = _pbm.expectedTimeToCompute(
				machines()[machineId].getTask(taskPos), machineId);

		if (minCostTaskValue > currentTaskCost) {
			minCostTaskValue = currentTaskCost;
			minCostTaskPos = taskPos;
		}
	}

	assert(machines()[machineId].countTasks() > minCostTaskPos);
	assert(minCostTaskPos >= 0);

	return minCostTaskPos;
}

int Solution::getMinDestinationCostTaskPosByMachine(int machineId,
		int destinationMachineId) const {

	// if (DEBUG) cout << endl << "[DEBUG] Solution::getMinDestinationCostTaskPosByMachine" << endl;
	if (machines()[machineId].countTasks() > 0) {
		int minCostTaskPos = 0;
		double minCostTaskValue = _pbm.expectedTimeToCompute(
				machines()[machineId].getTask(0), destinationMachineId);

		for (int taskPos = 1; taskPos < machines()[machineId].countTasks(); taskPos++) {
			double currentTaskCost;
			currentTaskCost = _pbm.expectedTimeToCompute(
					machines()[machineId].getTask(taskPos),
					destinationMachineId);

			if (minCostTaskValue > currentTaskCost) {
				minCostTaskValue = currentTaskCost;
				minCostTaskPos = taskPos;
			}
		}

		return minCostTaskPos;
	} else {
		return -1;
	}
}

Solution::~Solution() {
}

// UserStatistics -------------------------------------------------------

UserStatistics::UserStatistics() {
}

ostream& operator<<(ostream& os, const UserStatistics& userstat) {
	os << "\n---------------------------------------------------------------"
			<< endl;
	os << "                   STATISTICS OF TRIALS                   	 "
			<< endl;
	os << "------------------------------------------------------------------"
			<< endl;

	os << endl << "trial\t" << "best\t\t" << "worst\t\t\t" << "eval_best_found"
			<< "\t\t\t" << "iter_best_found" << "\t\t\t" << "time_best_found"
			<< "\t\t" << "time_spent_trial";

	for (int i = 0; i < userstat.result_trials.size(); i++) {
		os << endl << userstat.result_trials[i].trial << "\t"
				<< userstat.result_trials[i].best_cost_trial << "\t\t"
				<< userstat.result_trials[i].worst_cost_trial << "\t\t\t"
				<< userstat.result_trials[i].nb_evaluation_best_found_trial
				<< "\t\t\t"
				<< userstat.result_trials[i].nb_iteration_best_found_trial
				<< "\t\t\t" << userstat.result_trials[i].time_best_found_trial
				<< "\t\t" << userstat.result_trials[i].time_spent_trial;
	}
	os << endl
			<< "------------------------------------------------------------------"
			<< endl;
	return os;
}

UserStatistics& UserStatistics::operator=(const UserStatistics& userstats) {
	result_trials = userstats.result_trials;
	return (*this);
}

void UserStatistics::update(const Solver& solver) {
	if ((solver.pid() != 0) || (solver.end_trial() != true)
			|| ((solver.current_iteration()
					!= solver.setup().nb_evolution_steps()) && !terminateQ(
					solver.pbm(), solver, solver.setup())))
		return;

	struct user_stat *new_stat;

	if ((new_stat = (struct user_stat *) malloc(sizeof(struct user_stat)))
			== NULL)
		show_message(7);
	new_stat->trial = solver.current_trial();
	new_stat->nb_evaluation_best_found_trial
			= solver.evaluations_best_found_in_trial();
	new_stat->nb_iteration_best_found_trial
			= solver.iteration_best_found_in_trial();
	new_stat->worst_cost_trial = solver.worst_cost_trial();
	new_stat->best_cost_trial = solver.best_cost_trial();
	new_stat->time_best_found_trial = solver.time_best_found_trial();
	new_stat->time_spent_trial = solver.time_spent_trial();

	result_trials.append(*new_stat);
}

void UserStatistics::clear() {
	result_trials.remove();
}

UserStatistics::~UserStatistics() {
	result_trials.remove();
}

//  User_Operator:Intra_operator ---------------------------------------------------------

User_Operator::User_Operator(const unsigned int _number_op) :
	Intra_Operator(_number_op) {
}

void User_Operator::execute(Rarray<Solution*>& sols) const {
}

void User_Operator::setup(char line[MAX_BUFFER]) {
}

Intra_Operator *User_Operator::create(const unsigned int _number_op) {
	return new User_Operator(_number_op);
}

ostream& operator<<(ostream& os, const User_Operator& u_op) {
	os << "User Operator.";
	return os;
}

void User_Operator::RefreshState(const StateCenter& _sc) const {
}

void User_Operator::UpdateFromState(const StateCenter& _sc) {
}

User_Operator::~User_Operator() {
}

// StopCondition_1 -------------------------------------------------------------------------------------

StopCondition_1::StopCondition_1() :
	StopCondition() {
}

bool StopCondition_1::EvaluateCondition(const Problem& pbm,
		const Solver& solver, const SetUpParams& setup) {

	cout << "solver.time_spent_trial: " << solver.time_spent_trial() << endl;
	cout << "setup.timeout() * 1.0e+06: " << setup.timeout() * 1.0e+06 << endl;

	if (solver.time_spent_trial() >= (setup.timeout() * 1.0e+06)) {
		return true;
	} else {
		return false;
	}

	return false;
}

StopCondition_1::~StopCondition_1() {
}

//------------------------------------------------------------------------
// Specific methods ------------------------------------------------------
//------------------------------------------------------------------------

bool terminateQ(const Problem& pbm, const Solver& solver,
		const SetUpParams& setup) {

	StopCondition_1 stop;
	return stop.EvaluateCondition(pbm, solver, setup);

	return false;
}

}
#endif

