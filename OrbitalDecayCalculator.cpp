/*
Satellite Orbital Decay Calculator

Author: Danilo Nascimento
E-mail: me@daniloonspace.com

This is a simple model for atmospheric density as a function of space environmental parameters, and
shows how this may be applied to calculate decay rates and orbital lifetimes of satellites in essentially circular orbits
below 500 km altitude.
Low Earth orbiting satellites experience orbital decay and have physical lifetimes determined almost entirely by
their interaction with the atmosphere. Prediction of such lifetimes or of a re-entry date is of great interest to satellite
planners, users, trackers, and frequently to the general public.
Article "Satellite Orbital Decay Calculations": https://www.sws.bom.gov.au/Category/Educational/Space%20Weather/Space%20Weather%20Effects/SatelliteOrbitalDecayCalculations.pdf

Find out the specific F10.7 value at https://spaceweather.gc.ca/forecast-prevision/solar-solaire/solarflux/sx-5-flux-en.php
Find out the specific Ap (geomagnetic A Index) at https://www.spaceweatherlive.com/en/help/the-ap-index.html

More visual https://www.swpc.noaa.gov/communities/satellites


*/


#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

using namespace std;

// Constants
const double Me = 5.98e+24; // Earth mass
const double Re = 6378000;  // Earth radius
const double G = 6.67e-11;  // Universal constant of gravitation

int main()
{
    // Variables
    char satName[100];
    double mass, area, H, F10, Ap, T = 0.0, dT = 0.1;
    double H1 = 10.0, H2;
    double D9 = dT * 3600 * 24;
    double R, P;

    // Get input from the user
    cout << "Satellite name: ";
    cin >> satName;
    cout << "Satellite mass [kg]: ";
    cin >> mass;
    cout << "Satellite area [m^2]: ";
    cin >> area;
    cout << "Satellite Starting height/Altitude [km]: ";
    cin >> H;
    cout << "Solar Radio Flux (F10.7) in SFU: ";
    cin >> F10;
    cout << "Geomagnetic A index: ";
    cin >> Ap;

    // Calculate initial values
    H2 = H;
    R = Re + H * 1000;
    P = 2 * M_PI * sqrt(R * R * R / Me / G);

    // Log File Config
    string fileName = string(satName) + "_OrbitalDecay.csv";
    ofstream logFile(fileName);

    // Display initial values
    cout << "\nFile - " << satName << endl;
    cout << "\t Mass = " << mass << " Kg" << endl;
    cout << "\t Area = " << area << " m^2" << endl;
    cout << "\t Initial height = " << H << " Km" << endl;
    cout << "\t F10.7 = " << F10 << " , "
         << "Ap = " << Ap << endl;

    // Write initial values to log file
    logFile << "File - " << satName << endl;
    logFile << "\t Mass = " << mass << " Kg" << endl;
    logFile << "\t Area = " << area << " m^2" << endl;
    logFile << "\t Initial height = " << H << " Km" << endl;
    logFile << "\t F10.7 = " << F10 << " , "
            << "Ap = " << Ap << endl;

    // Display and write column headers to log file
    cout << "\nTime\t\tHeight\t\tPeriod\t\tMean motion\t\tDecay\n";
    cout << "(days)\t\t(km)\t\t(mins)\t\t(rev/day)\t\t(rev/day^2)\n";
    logFile << "\nTime,Height,Period,Mean motion,Decay\n";
    logFile << "(days),(km),(mins),(rev/day),(rev/day^2)\n";

    // Calculate orbital decay
    while (true)
    {
        double SH = (900 + 2.5 * (F10 - 70) + 1.5 * Ap) / (27 - 0.012 * (H - 200));
        double DN = 6.0e-10 * exp(-(H - 175) / SH);         // atmospheric density
        double dP = 3 * M_PI * (area / mass) * R * DN * D9; // decrement in orbital period

        if (H <= H2)
        {
            double Pm = P / 60;
            double MM = 1440 / Pm;
            double Decay = dP / dT / P * MM; // rev/day/day
            cout << T << "\t\t" << H << "\t\t" << P << "\t\t" << MM << "\t\t" << Decay << '\n';
            logFile << T << "," << H << "," << P << "," << MM << "," << Decay << '\n';
            H2 -= H1; // decrement print height
        }
        if (H < 180) // If Orbit is less than 180 KM end estimation
            break;

        P -= dP;
        T += dT;
        R = pow(G * Me * P * P / 4 / M_PI / M_PI, 1.0 / 3.0); // new orbital radius
        H = (R - Re) / 1000;                                  // new altitude (semimajor axis)
    }

    // Display and write re-entry information to log file
    cout << "Re-entry after " << T << " days";
    logFile << "Re-entry after " << T << " days";

    // Close log file
    logFile.close();
    return 0;
}
