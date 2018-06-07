#include <SFML/Graphics.hpp>
#include "geometry.h"
#include "window.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

vector<geometry::Point> GetInitialSolution(const vector<geometry::Point>& points) {
    // Find one of the leftest and one of the rightest points
    int n = points.size();
    int leftest = 0, rightest = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].x < points[leftest].x) leftest = i;
        if (points[i].x > points[rightest].x) rightest = i;
    }

    // Split points in two sets: above and below line leftest-rightest
    vector<geometry::Point> above;
    vector<geometry::Point> below;
    for (int i = 0; i < n; i++) {
        if (i == leftest || i == rightest) continue;
        if (geometry::Turn(points[leftest], points[rightest], points[i]) == 1) {
            above.push_back(points[i]);
        } else {
            below.push_back(points[i]);
        }
    }

    // Sort sets
    sort(above.begin(), above.end(), 
         [](const geometry::Point& a, const geometry::Point& b) {
             if (a.x == b.x) return a.y < b.y;
             return a.x < b.x;
         }
    );
    sort(below.begin(), below.end(), 
         [](const geometry::Point& a, const geometry::Point& b) {
             if (a.x == b.x) return a.y < b.y;
             return a.x > b.x;
         }
    );

    // Stitch the final polygon: leftest->above->rightest->below
    vector<geometry::Point> poly;
    poly.push_back(points[leftest]);
    poly.insert(poly.end(), above.begin(), above.end());
    poly.push_back(points[rightest]);
    poly.insert(poly.end(), below.begin(), below.end());
    return poly;
}

vector<geometry::Point> SimulatedAnnealing(const vector<geometry::Point>& points, int steps, 
                                           int start_temp, Window& window, int test_number, bool use_three_swaps) {
    vector<geometry::Point> poly = points;
    int n = poly.size();
    double curr_score = geometry::PolyArea(poly);
    double best_score = curr_score;
    double stop_temp = 0;
    double temp = start_temp;

    // Drawing variable
    const int max_checkpoints = 100;  // Don't draw/print more than 100 checkpoints
    int mod = steps/max_checkpoints;

    // Do annealing
    for (int step = 1; step <= steps; step++) {
        // Cooling schedule
        // Non-monotonic adaptive cooling based on linear multiplicative cooling
        double expo = (2.0 * log(start_temp - stop_temp) / steps) * (step - steps/2.0);
        double tk = stop_temp + (start_temp - stop_temp) * (1 / (1 + exp(expo)));
        double tk_lin = steps / (1 + 0.8 * step);
        double m = 1 + (curr_score - best_score) / curr_score;
        temp = m * tk_lin;

        // Choose swap type
        bool three_swap;
        if (use_three_swaps && rand()%2 == 0) {
            three_swap = true; 
        } else {
            three_swap = false;
        }

        // Swap random vertices
        int i, j, k;
        if (three_swap) {
            // Three swap
            i = rand() % n, j = rand() % n, k = rand() % n;
            geometry::Point tmp = poly[i];
            poly[i] = poly[j], poly[j] = poly[k], poly[k] = tmp;
            while (geometry::IsSelfIntersectingPoly(poly)) {
                geometry::Point tmp = poly[k];
                poly[k] = poly[j], poly[j] = poly[i], poly[i] = tmp;
                i = rand() % n, j = rand() % n, k = rand() % n;
                tmp = poly[i];
                poly[i] = poly[j], poly[j] = poly[k], poly[k] = tmp;
            } 
        } else {
            // Two swap
            i = rand() % n, j = rand() % n;
            swap(poly[i], poly[j]);
            while (geometry::IsSelfIntersectingPoly(poly)) {
                swap(poly[i], poly[j]);
                i = rand() % n, j = rand() % n;
                swap(poly[i], poly[j]);
            } 
        }
        
        // Move to new state or keep the old
        double nxt_score = geometry::PolyArea(poly);
        if (nxt_score < curr_score) {
            curr_score = nxt_score;
        } else {
            double p = exp((curr_score - nxt_score) / temp);
            double r = (double)rand() / RAND_MAX;
            if (r < p) {
                curr_score = nxt_score;
            } else {
                // Roll back the swap
                if (three_swap) {
                    geometry::Point tmp = poly[k];
                    poly[k] = poly[j], poly[j] = poly[i], poly[i] = tmp;
                } else {
                    swap(poly[i], poly[j]);
                }
            }
        }

        // Update the best score
        best_score = min(best_score, curr_score);

        // Draw and print output
        stringstream buffer;
        buffer << "|Test Case #" << test_number << endl;
        buffer << "|Area: " << setw(21) << fixed << setprecision(1) << curr_score << endl;
        buffer << "|Best Area Seen: " << setw(11) << fixed << setprecision(1) << best_score << endl;
        buffer << "|Iteration: " << setw(16) << step << " (out of " << steps << ")" << endl;
        buffer << "|Temperature: " << setw(14) << fixed << setprecision(3) << temp << endl;

        if (mod == 0 || step == 1 || step%mod == 0) {
            window.Draw(poly, buffer.str());
            cout << buffer.str() << endl;
        }
    }
    return poly;
}

// Params:
// ./main <test_number> <run_number> <starting_temp> <num_steps> <use_three_swaps>
int main(int argc, char* argv[]) {
    srand(time(NULL));
    
    // Grab command-line arguments
    if (argc != 6) {
        cerr << "Error: there should be exactly 5 command-line arguments." << endl;
    }
    int test_number = atoi(argv[1]);
    int run_number = atoi(argv[2]);
    int start_temp = atoi(argv[3]);
    int annealing_steps = atoi(argv[4]);
    int use_three_swaps = atoi(argv[5]);

    // Load the testcase
    int t;
    vector<geometry::Point> points;

    stringstream buffer;
    buffer << "testcases/test" << test_number << ".txt";
    ifstream in_file(buffer.str());
    int n;
    in_file >> n;
    for (int i = 0; i < n; i++) {
        double x, y;
        in_file >> x >> y;
        points.push_back({x, y});
    }

    // Prepare GUI
    Window window(20);

    // Get initial solution
    vector<geometry::Point> poly = GetInitialSolution(points);
    poly = SimulatedAnnealing(poly, annealing_steps, start_temp, window, test_number, use_three_swaps);

    // Output results to a file
    buffer.str("");
    buffer << "out/test" << test_number << "_" << run_number << ".txt";
    ofstream log(buffer.str());

    double area = geometry::PolyArea(poly);
    log << "Area: " << area << endl;
    log << "Vertices: ";
    for (const geometry::Point& pt : poly) {
        log << "(" << pt.x << " " << pt.y << ") ";
    }
    log << endl;

    // Save a photo of the final result and keep the window open
    buffer.str("");
    buffer << "out/test" << test_number << "_" << run_number << ".png";
    window.SaveToPhoto(buffer.str());
    window.Wait();
    return 0;
}