// Travelling Salesman Problem (TSP) (NP-hard Problem) using Simulated Annealing Algorithm

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <random>
#include <ctime>

using namespace std;

class Point{
    public:
    int x;
    int y;
    int index;
    Point(int ind, int xcood, int ycood):index(ind), x(xcood), y(ycood){}
};

class PointReader{
    private:
        vector<Point> S_points;
    public:
        bool read_points(const string& f_name){
            ifstream infile(f_name);
            if(!infile.is_open()){
                cerr<<"Error: Could not open file:"<<f_name<<endl;
                return false;
            }

            int index,x,y;
            while(infile>>index>>x>>y){
                S_points.push_back(Point(index, x, y));
            }

            infile.close();
            return true;
        }

        //Function to return the data points
        const vector<Point>& getS_points() const{
            return S_points;
        } 

        // Function to display the points
        void display_points(){
            for(const auto& p:S_points){
                cout<<"index:"<<p.index<<", x:"<<p.x<<", y:"<<p.y<<endl;
            }
        }
};


// Function to calculate the Euclidean Distance
double cal_Euc_Dis(const Point& p1, const Point& p2){
    return sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2));
}

// Function to calcuate Total distance traversed during the entire tour
double cal_Total_Dis(const vector<Point>& s_points, const vector<int>& tour){
    double totalDistance=0;
    for(size_t i=0; i<tour.size()-1;i++){
        totalDistance+=cal_Euc_Dis(s_points[tour[i]],s_points[tour[i+1]]);
    }

    // Add distance to return to starting point from last vertex.
    totalDistance+=cal_Euc_Dis(s_points[tour.back()], s_points[tour[0]]);
    return totalDistance;
}

// Function to Implement TSP using Simulated Annealing
vector<int> simulateAnnealing(const vector<Point>& s_points, int iteration, double initialTmp, double coolingRate){
    int numPoints=s_points.size();
    vector<int> currentTour(numPoints);
    iota(currentTour.begin(), currentTour.end(),0); // Initialize tour with indices 0,1,2,...,n-1

    // Randomly shuffle the initial tour
    random_device rd;
    mt19937 g(rd());
    shuffle(currentTour.begin(), currentTour.end(),g);

    double currentDistance=cal_Total_Dis(s_points,currentTour);
    vector<int> bestTour=currentTour;
    double bestDistance=currentDistance;

    double temp=initialTmp;

    for(int iter=0;iter<iteration;iter++){
        int i=rand()%numPoints;
        int j=(i+1+rand()%(numPoints-1))%numPoints;
        swap(currentTour[i],currentTour[j]);

        double new_Distance=cal_Total_Dis(s_points,currentTour);

        // Accept some worse solutions based upon probability and cooling rate.
        double delta = new_Distance-currentDistance;

        //rand()/static_cast<double>(RAND_MAX) is used to generate random numbers between 0 and 1. 
        if(delta<0|| (exp(-delta/temp) > (rand()/static_cast<double>(RAND_MAX))) ){
            currentDistance=new_Distance;
            
            if(currentDistance<bestDistance){
                bestTour=currentTour;
                bestDistance=currentDistance;
            }
        }
        else{
            // If it is not accepted then swap them again
            swap(currentTour[i],currentTour[j]);
        }

        //Cooling rate of temperature
        temp*=coolingRate;
            
        // To Print progress after every 1000 iterations
        if(iter%1000==0){
            cout<<"Iteration "<<iter <<"- Best Distance: "<<bestDistance<<endl;
        }
    }

    return bestTour;
}

// Function To Export the best tour to CSV file
void exportTourtoCSV(const vector<Point>& s_points, const vector<int>& tour, const string& f_name){
    ofstream outfile(f_name);
    if(!outfile.is_open()){
        cerr<<"Error: Could not open file "<< f_name<<" for writing."<<endl;
        return ;
    }

    outfile<<"Index,X,Y\n";
    for(int idx:tour){
        outfile<<s_points[idx].index<<","<<s_points[idx].x<<","<<s_points[idx-1].y<<"\n";
    }

    // Adding the return to the first point
    outfile<< s_points[tour[0]].index<<","<<s_points[tour[0]].x<<", "<<s_points[tour[0]].y<<"\n";

    outfile.close();
}

int main(){
    PointReader reader;

    // Reading the points from the file 
    if(reader.read_points("380 tsp points.txt")){
        cout<<"Points read successfully"<<endl;
    }
    else{
        cerr<<"Error: Could not read points"<<endl;
        return -1;
    }

    const vector<Point>& points=reader.getS_points();

    // Simulated Annealing Parameters
    int iterations=10000;
    double initialTmp=1000;
    double coolingRate=0.80;

    // Simulated Annealing
    vector<int> bestTour = simulateAnnealing(points, iterations, initialTmp, coolingRate);

    // Output of the best tour
    cout<<"Best tour found: "<<endl;
    for(int idx:bestTour){
        cout<<(idx+1)<<" ";
    }
    cout<<endl;

    // Best distance
    cout<<"Best Distance: "<<cal_Total_Dis(points, bestTour)<<endl;

    // To export the best tour to a CSV file
    exportTourtoCSV(points, bestTour, "best_tour.csv");

    return 0;
}
