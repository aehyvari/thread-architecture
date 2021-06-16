# OpenSMT communication

This repository serves for discussing an architecture for communication
between OpenSMT solver, communication thread (CT), Clause Cloud (CC) and SMTS.

At the moment there's a prototype implementation of a channel that
supports:
 - opensmt signalling to CT that it has new clauses to
   share
 - opensmt signalling to CT that it will stop
 - CT to opensmt that opensmt should stop

The idea is that CT will wait on a condition variable for signals coming
from opensmt, SMTS, and possibly a timer that indicates that clauses
should be pushed to and queried from CC.

Next steps would be:
 - See if this is a promising architecture or if something should be
   changed
 - Implement the communication between (a simulator for) SMTS and
   CT
 - Impelement the communication between CC and CT
