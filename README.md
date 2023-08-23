# reversi
The "reversi" game project is a project of Master 1 Cryptology and Security and IT.\
The rules of the game reverse see https://samsoft.org.uk/reversi/strategy.htm. \
To start the game, on the root:

    make
  
then do:

    ./reversi

## Play a reversi game with human or program players:
        -s, --size SIZE		board size(min=1, max=5 (default=4))
        -b, --black-ai [N]	set tactic of black player (default: 0)
        -w, --white-ai [N]	set tactic of white player (default: 0)
        -c, --contest		enable 'contest' mode
        -v, --verbose		verbose output
        -V, --version		display version and exit
        -h, --help		display this help and exit
    Tactic list: human (0), random (1) 
    
## Example with the 5 sizes

We can see the examples :
        
        test_reversi_s_1_w_1_b_1.txt -> -s 1 -w 1 -b 1 
        test_reversi_s_2_w_1_b_1.txt -> -s 2 -w 1 -b 1
        test_reversi_s_3_w_1_b_1.txt -> -s 3 -w 1 -b 1
        test_reversi_s_4_w_1_b_1.txt -> -s 4 -w 1 -b 1
        test_reversi_s_5_w_1_b_1.txt -> -s 5 -w 1 -b 1
