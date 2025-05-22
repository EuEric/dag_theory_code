// #include "dag_parser.h"    

// void assignWCET(Graph g,const int minC, const int maxC){
//     for(auto &t: g.tasks)
//         t.wcet = intRandMaxMin(minC, maxC);
// }

// void expandTaskSeriesParallel(Graph g, Task* source, Task* sink, const int depth, const int numBranches, const bool ifCond, GeneratorParams& gp){

//     int depthFactor = std::max(gp.maxCondBranches, gp.maxParBranches);
//     int horSpace = std::pow(depthFactor,depth);

//     if(source && sink){
//         Task *so = new Task; //source
//         Task *si = new Task; //sink
//         so->depth = depth;
//         si->depth = -depth;
//         so->width = 0;
//         so->width = 0;
//         si->id = 1;

//         g.tasks.push_back(*so);
//         g.tasks.push_back(*si);
//         // V.push_back(so);
//         // V.push_back(si);

//         double r = ((double) rand() / (RAND_MAX));
//         if (r < gp.probSCond){ //make it conditional
//             int cond_branches = intRandMaxMin(2, gp.maxCondBranches);
//             expandTaskSeriesParallel(g, &(g.tasks[0]), &(g.tasks[1]), depth - 1, cond_branches, true, gp);
//         }
//         else{
//             int par_branches = intRandMaxMin(2, gp.maxParBranches);
//             expandTaskSeriesParallel(g, &(g.tasks[0]), &(g.tasks[1]), depth - 1, par_branches, false, gp);
//         }
//     }
//     else{
//         float step = horSpace / (numBranches - 1);
//         float w1 = (source->width - horSpace / 2);
//         float w2 = (sink->width - horSpace / 2);

//         for(int i=0; i<numBranches; ++i){
            
//             // int current = V.size();
//             creationStates state = TERMINAL_T;
//             if (depth != 0) state = static_cast<creationStates>(gp.dist(gp.gen));

//             switch (state){
//             case TERMINAL_T:{
//                 Task *v = new Task;
//                 v->id = g.tasks.size();
//                 g.add_edge(source->id, v->id);

//                 // v->pred.push_back(source);
//                 // v->succ.push_back(sink);
//                 // v->mode = ifCond? C_INTERN_T : NORMAL_T;
//                 v->depth = depth;
//                 v->width = w1 + step * (i - 1);

//                 g.tasks.push_back(*v);
//                 //V.push_back(v);

//                 source->mode    = ifCond ? C_SOURCE_T : NORMAL_T;
//                 sink->mode      = ifCond ? C_SINK_T : NORMAL_T;
//                 g.add_edge(source->id, (g.tasks[g.tasks.size() - 1]).id);
//                 //source->succ.push_back(V[V.size()-1]);
//                 g.add_edge((g.tasks[g.tasks.size() - 1]).id, sink->id);
//                 //sink->pred.push_back(V[V.size()-1]);
//                 break;
//             }
//             case PARALLEL_T: case CONDITIONAL_T:{
//                 Task *v1 = new Task;
//                 v1->id = g.tasks.size();
//                 //v1->pred.push_back(source);
//                 g.add_edge(source->id, v1->id);
//                 v1->mode = ifCond? C_INTERN_T : NORMAL_T;
//                 v1->depth = depth;
//                 v1->width = w1 + step * (i - 1);
//                 g.tasks.push_back(*v1);
//                 //V.push_back(v1);

//                 //source->succ.push_back(V[V.size()-1]);
//                 g.add_edge(source->id, (g.tasks[g.tasks.size() - 1]).id);
//                 source->mode = ifCond ? C_SOURCE_T : NORMAL_T;

//                 Task *v2 = new Task;
//                 v2->id = g.tasks.size();
//                 //v2->succ.push_back(sink);
//                 g.add_edge(v2->id,sink->id);
//                 v2->mode = ifCond? C_INTERN_T : NORMAL_T;
//                 v2->depth = -depth;
//                 v2->width = w2 + step * (i - 1);
//                 //V.push_back(v2);
//                 g.tasks.push_back(*v2);

//                 // sink->pred.push_back(V[V.size()-1]);
//                 g.add_edge((g.tasks[g.tasks.size() - 1]).id,sink->id);
//                 sink->mode = ifCond ? C_SINK_T : NORMAL_T;

//                 int max_branches = (state == PARALLEL_T )? gp.maxParBranches : gp.maxCondBranches;
//                 float cond = (state == PARALLEL_T) ? false: true;

//                 int branches = intRandMaxMin(2, max_branches);
            
//                 expandTaskSeriesParallel(g, &(g.tasks[g.tasks.size() - 2]), &(g.tasks[g.tasks.size() - 1]), depth - 1, branches, cond, gp);
            
//                 break;
//             }
            
//             default:
//                 break;
//             }
//         }
//     }
// }

// void transitiveReduction(Graph g){
//     for(int i=0; i<g.tasks.size(); ++i){
//         // std::vector<SubTask*> succ_desc;
//         vector<Task> descendants;
//         g.get_descendants(g.tasks[i].id);
//         //find all the successors descendants
//         // for(int j=0; j<V[i]->succ.size();++j){
//         //     std::vector<SubTask*> succ_desc_j = getSubTaskDescendants(V[i]->succ[j]->id);
//         //     succ_desc.insert(succ_desc.end(), succ_desc_j.begin(), succ_desc_j.end());
//         // }
        
//         // is a successor is also a successors descendants, mark to remove
//         std::vector<Task> to_remove;
//         vector<int> successors = g.successors(g.tasks[i].id);
//         for(int j = 0; j < successors.size(); j++) {
//             if ( std::find(descendants.begin(), descendants.end(), successors[j]) != descendants.end() ){
//                 to_remove.push_back(g.tasks[successors[j]]);
//             }
//         }

//         //remove 
//         // Our remove only removes a node, but not a task itself!
//         for(auto& r:to_remove){
//             g.remove_node(r.id);
//             //V[i]->succ.erase(std::remove(V[i]->succ.begin(), V[i]->succ.end(), r), V[i]->succ.end());
//             //r->pred.erase(std::remove(r->pred.begin(), r->pred.end(), V[i]), r->pred.end());
//         }
//     }
// }

// float UUniFast_Upart(Graph g, float& sum_U, const int i, const int n_tasks){
//     float next_sum_U=0, U_part = 0;
//     double r;
//     if(i < n_tasks-1){
//         r = ((double) rand() / (RAND_MAX));
//         next_sum_U = sum_U * std::pow(r, 1./(n_tasks-i-1));
//         U_part = sum_U - next_sum_U;

//         // longest chain
//         while(getLength() > std::ceil(getWCW() / U_part)){
//             r = ((double) rand() / (RAND_MAX));
//             next_sum_U = sum_U * std::pow(r, 1./(n_tasks-i-1));
//             U_part = sum_U - next_sum_U;
//         }
//         sum_U = next_sum_U;
//     }
//     else
//         U_part = sum_U;

//     return U_part;
// }

// enum creationStates {CONDITIONAL_T=0, PARALLEL_T=1, TERMINAL_T=2};

// //TODO: work on a function that generates a single dag for now..
// // Might need to assign a maximum utilization??
// void generate_DAG_Melani(Graph g, const int nr_proc, GeneratorParams& gp) {
//     // float U_part = 0;
//     // float sum_U = U_tot;

//     Graph g;

//     //Move the DAGTask things somewhere else (probably in the generation part)...
//         expandTaskSeriesParallel(g, nullptr, nullptr, gp.recDepth, 0, false,gp);
//         assignWCET(g, gp.Cmin, gp.Cmax);
//         //Not applicable
//         // if( !(  gp.aType == AlgorithmType_t::FTP 
//         //         && gp.DAGType ==DAGType_t::DAG ) || gp.sType == SchedulingType_t::PARTITIONED )
//         //     t.makeItDag(gp.addProb);

//         transitiveReduction(g);

//         //t.computeWorstCaseWorkload();
//         //t.computeVolume();
//         //t.computeLength();

//         //random assignment of core in partitioned case
//         // Not aplicable here to have a TDAG? or partitioned case
//         // auto V = t.getVertices();
//         // for(int j=0; j<V.size(); ++j)
//         //     V[j]->core = rand() % n_proc;

//         // if(gp.DAGType == DAGType_t::TDAG){
//         //     //random assignment of core types to subnodes
//         //     for(int j=0; j<V.size(); ++j)
//         //         V[j]->gamma = rand() % gp.typedProc.size();
//         // }

//         if(gp.gType == GenerationType_t::VARYING_N){
//             float U_part = UUniFast_Upart(sum_U, i, n_tasks, t) ;
//             // std::cout<<"U_part "<<U_part<<std::endl;
//             t.assignSchedParametersUUniFast(U_part);
//             if(gp.dtype == DeadlinesType_t::IMPLICIT)
//                 t.setDeadline(t.getPeriod());
//             t.computeDensity();
//             t.computeUtilization();
//             U += t.getWCW() / t.getPeriod();
//         }
//         else{
//             if(n_tasks == 1){
//                 float t_to_assign = std::floor(t.getWCW() / U_tot);
//                 float d_to_assign = floatRandMaxMin(std::min(t.getLength(), t_to_assign), t_to_assign);
//                 t.assignFixedSchedParameters(t_to_assign, d_to_assign);
//                 if(gp.dtype == DeadlinesType_t::IMPLICIT)
//                     t.setDeadline(t.getPeriod());

//                 t.computeDensity();
//                 t.computeUtilization();
//                 U += t.getWCW() / t.getPeriod();
//             }
//             else{

//                 t.assignSchedParameters(gp.beta);
//                 if(gp.dtype == DeadlinesType_t::IMPLICIT)
//                     t.setDeadline(t.getPeriod());

//                 t.computeDensity();
//                 t.computeUtilization();
//                 U += t.getWCW() / t.getPeriod();

//                 if( U > U_tot || i == n_tasks - 1){
//                     float U_prev = U - t.getWCW() / t.getPeriod();
//                     float U_target = U_tot - U_prev;
//                     float t_to_assign = std::floor(t.getWCW() / U_target);
//                     float d_to_assign = floatRandMaxMin(std::min(t.getLength(), t_to_assign), t_to_assign);;
//                     if(gp.dtype == DeadlinesType_t::IMPLICIT)
//                         d_to_assign = t_to_assign;
//                     t.assignFixedSchedParameters(t_to_assign, d_to_assign);
//                     t.computeDensity();
//                     t.computeUtilization();
//                     U = U_prev + t.getWCW() / t.getPeriod();
//                     n_tasks = i;
//                     tasks.push_back(t);
//                     break;
//                 }
//             }
//         }

//         tasks.push_back(t);


//     // computeUtilization();
//     //computeHyperPeriod();
//     //computeMaxDensity();
// }
    
// // Function for enerating a DAG using Melani technique?
// // We no longer track U_tot because the consider the dags as independent and only want to analyze one dag at a time.
// void generate_DAGS_Melani(int nr_dags, const int nr_proc, GeneratorParams& gp) {
//     for(int i=0; i<nr_dags; ++i){
//         Graph g;
//         generate_DAG_Melani(g, nr_proc, gp);
//     }
// }
