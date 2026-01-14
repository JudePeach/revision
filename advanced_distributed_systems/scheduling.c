#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * This program simulates job arrivals and scheduling on a set of servers
 * with different operating frequencies. Jobs arrive randomly over time,
 * are assigned to servers, and execution time depends on server frequency.
 */

int max(int a, int b);   // forward declaration

int main(int argc, char **argv)
{
    /* Loop and index variables */
    int i, j, m, s, t, u;

    /* Simulation parameters */
    int jobs;                 // total number of jobs scheduled
    int duration;             // total simulation time
    int servers;              // number of servers
    int arrival_rate;         // average arrival rate of jobs
    int max_execution;        // maximum possible execution time
    int first_job;            // time when the first job arrives

    /* Scheduling-related variables */
    int eft;                  // earliest finish time
    int task_duration;        // execution time of a job on a server
    int random_schedule = 0;  // flag for random vs. greedy scheduling

    /* Job-specific variables */
    float job_factor;         // job-specific scaling factor
    float job_duration;       // base execution time of a job

    /* Energy and utilization */
    float energy;
    int util[100];            // utilization time per server

    /* Server-related arrays */
    int server[100];          // next available time for each server
    float freq[100];          // frequency of each server

    /* Random number */
    double my_rnd;

    /* Seed the random number generator */
    srand(time(0));

    /* Initialize simulation parameters */
    servers  = 100;
    duration = 10000;

    /* Read command-line arguments */
    arrival_rate  = atoi(argv[1]);
    max_execution = atoi(argv[2]);

    /* Validate input */
    if (max_execution < 10) {
        printf("max execution should be at least 10\n");
        exit(0);
    }

    jobs = 0;

    /*
     * Initialize servers:
     * - server[i] = next available time
     * - freq[i]   = operating frequency (0.6 to 1.0)
     * - util[i]   = total busy time
     */
    for (i = 0; i < servers; i++) {
        server[i] = 0;
        freq[i]   = 0.6 + 0.1 * (int)(i / 20);
        util[i]   = 0;
    }

    /*
     * Main simulation loop:
     * Each iteration represents one unit of time
     */
    for (t = 1; t <= duration; t++) {

        /* -------------------------------
         * PART 1: Job arrival
         * -------------------------------
         * Generate a random number in [0,1).
         * If it is below 1/arrival_rate, a job arrives.
         */
        my_rnd = (double)rand() / ((double)RAND_MAX + 1.0);

        if (my_rnd < (double)(1.0 / arrival_rate)) {

            /* A job has arrived */
            jobs++;
            if (jobs == 1) {
                first_job = t;
            }

            /* Random base execution time between 10 and max_execution */
            job_duration = 10.0 +
                (max_execution - 10.0) *
                (double)rand() / ((double)RAND_MAX + 1.0);

            /* Random job factor in [0,1) */
            job_factor = (double)rand() / ((double)RAND_MAX + 1.0);

            /* -------------------------------
             * PART 2: Scheduling decision
             * -------------------------------
             * Either randomly select a server,
             * or choose the one with earliest finish time.
             */
            if (random_schedule) {

                /* Random server selection */
                my_rnd = 100 * (double)rand() / ((double)RAND_MAX + 1.0);
                s = (int)my_rnd;

                if (s > servers - 1) {
                    printf("warning, server out of bounds\n");
                }

                task_duration =
                    (1.0 + job_factor * (1.0 / freq[s] - 1.0)) *
                    job_duration;

            } else {

                /* Greedy scheduling: earliest finish time (EFT) */
                s = 0;
                eft =
                    (1.0 + job_factor * (1.0 / freq[s] - 1.0)) *
                    job_duration +
                    max(t, server[0]);

                for (i = 0; i < servers; i++) {
					// compute the execution time on each serve
                    task_duration =
                        (1.0 + job_factor * (1.0 / freq[i] - 1.0)) *
                        job_duration;

						// if the execution on server i finishes earlier, update s and eft
                    if (task_duration + max(t, server[i]) < eft) {
                        s = i;
                        eft = task_duration + max(t, server[0]);
                    }
                }
            }

            /* -------------------------------
             * PART 3: Bookkeeping
             * -------------------------------
             * Update utilization and next available time
             */
            util[s] += task_duration;

            if (server[s] < t) {
                server[s] = t;
            }

            server[s] += task_duration;
        }
    }

    /* -------------------------------
     * Final statistics
     * -------------------------------
     */
    printf("Number of Jobs Scheduled: %d\n", jobs);

    m = 0;   // last completion time
    u = 0;   // total utilization

    for (i = 0; i < servers; i++) {
        if (server[i] > m) {
            m = server[i];
        }
        u += util[i];
    }

    printf("First job arrived at time: %d\n", first_job);
    printf("Last job finished at: %d\n", m);

    if (m > duration) {
        duration = m;
    }

    printf("Overall utilization: %1.12f\n",
           (float)((u + 0.0) / (duration * servers)));

    /* Energy calculation:
     * - Active energy depends on frequency
     * - Idle energy is constant (0.4)
     */
    energy = 0;
    for (i = 0; i < servers; i++) {
        energy += (float)util[i] * freq[i]
                  + (float)(0.4 * (duration - util[i]));
    }

    printf("Energy: %f\n\n", energy);

    return 0;
}

/*
 * Utility function: return maximum of two integers
 */
int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}
