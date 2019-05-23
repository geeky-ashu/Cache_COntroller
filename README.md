# Cache_COntroller
Objective:
==========
The main aim of the project is to determine the best architecture for the cache controller thus in order to improve its speed using Cholesky decomposition methods. The Cholesky operates on a 256x256 symmetric matrix.

Specification:
==============
•	Burst Length (BL): - 1,2, 4,8
•	Number of Ways(N): - 1,2,4,8,16
•	Write strategy (WS): - Write Back, Write Through Allocate, Write Through Non-Allocate.
•	Replacement Strategy: - Least Recently Used (LRU).

BEST & WORST CASE
=================

                         Best Case's
---------------------------------------------------------------
BEST CASE - 1    	 |  BEST CASE - 2	      |   BEST CASE - 3
N = 2	             |     N = 4	          |      N = 8
BL = 1	           |     BL = 1	          |      BL = 1
WS = 3	           |     WS = 3           |      WS = 3
Total access time  | Total access time =  |	Total access time =
= 1128635908	     |    1128635908        |    1128635908       
-----------------------------------------------------------------


                           Worst Case's
------------------------------------------------------------------
WORST CASE - 1	    |     WORST  CASE - 2	   |     WORST  CASE - 3
N = 1	              |         N = 2	         |          N = 1
BL = 2	            |         BL = 2	       |          BL = 4
WS =2	              |         WS =2	         |          WS =2
Total access time = |  Total access time =   |   Total access time =
1610142398          |     1610141410	       |        1586459654
--------------------------------------------------------------------
