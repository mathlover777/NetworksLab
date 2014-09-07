# sscanf(argv[1],"%lf",&a1);
# sscanf(argv[2],"%lf",&b1);
# sscanf(argv[3],"%lf",&f1);
# sscanf(argv[4],"%lf",&ampMax1);
# sscanf(argv[5],"%d",&n1);
# sscanf(argv[6],"%lf",&a2);
# sscanf(argv[7],"%lf",&b2);
# sscanf(argv[8],"%lf",&f2);
# sscanf(argv[9],"%lf",&ampMax2);
# sscanf(argv[10],"%d",&n2);
# sscanf(argv[11],"%lf",&t);
# sscanf(argv[12],"%lf",&fSample);
# sscanf(argv[13],"%lf",&band);
reset
g++ asg2.c -o asg2 `pkg-config --cflags --libs gsl`
./asg2 0 1000 5000 10 5 0 1000 8000 10 6 .1 20000 1000