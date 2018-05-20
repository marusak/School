{-
 - dfa-2-mka Convert deterministic finite automaton to minimized one
 - Author: Matej Marusak, xmarus06@stud.fit.vutbr.cz
 - Created as school project for FLP
 - February 2018
 -}

import System.IO
import Data.List
import Data.Ord
import Data.List.Split
import System.Environment

-- Type class of one rule
data Rule = Rule { start :: String
                 , symbol :: String
                 , end :: String
                 } deriving (Eq)

-- Type class of finite state automata
data DFA = DFA { states :: [String]
               , alphabet :: [String]
               , rules :: [Rule]
               , initState :: String
               , finalStates :: [String]
               } deriving (Eq)


main :: IO ()
main = do
    args <- getArgs
    case args of
        [ "-i", x ] -> do
            rawA <- readFile x
            printA $ interpreteA $ lines rawA
        [ "-t", x ] -> do
            rawA <- readFile x
            printA $ minimizeA $ completeA $ interpreteA $ lines rawA
        [ "-i" ]    -> do
            rawA <- getContents
            printA $ interpreteA $ lines rawA
        [ "-t" ]    -> do
            rawA <- getContents
            printA $ minimizeA $ completeA $ interpreteA $ lines rawA
        _   -> putStrLn "Usage: dka-2-mka -i|-t [input_file]"


-- Convert file content into internal data structure
interpreteA :: [String] -> DFA
interpreteA (states:initS:finalS:rules) =
    DFA getStates (nub $ map getAlphabet rules) (map getRule rules) (notEmpty $ isState initS) getFinalStates
    where
        getStates = splitOn "," states
        notEmpty "" = error "Initial state must be defined"
        notEmpty x = x
        isState "" = ""
        isState x = if x `elem` getStates
                    then x
                    else error "Unknown state used"
        getFinalStates = map isState $ splitOn "," finalS
        getRule t = getRule_ $ splitOn "," t
        getRule_ [f, s, t] = if f `elem` getStates && t `elem` getStates
                             then Rule f s t
                             else error "Rule contains unknown state"
        getRule_ [""] = error "Empty rule found"
        getRule_ _ = error "Invalid rule found"
        getAlphabet a = getAlpha $ splitOn "," a
        getAlpha [_, a, _] = a
        getAlpha [""] = error "Empty rule found"
        getAlpha _ = error "Invalid rule found"

-- Print internal data structure as valid input file
printA :: DFA -> IO ()
printA DFA {states = s, alphabet = a, rules = r, initState = i, finalStates = f} =
    putStr output
    where
        output = unlines $ [intercalate "," s, i, intercalate "," f] ++ map printRule r
        printRule Rule {start = s, symbol = s_, end = e} = intercalate "," [s, s_, e]

-- Convert DFA to MFA
minimizeA :: DFA -> DFA
minimizeA DFA {states = s, alphabet = a, rules = r, initState = i, finalStates = f} =
    DFA (convertStates classes s) a (nub $ map (convertRule classes) r)
        (convertState classes i) (convertStates classes f)
    where
        zIndi = [f, s \\ f]
        classes = getClasses zIndi r

convertStates :: [[String]] -> [String] -> [String]
convertStates classes = nub . map (convertState classes)

-- Get new state according to classes of indistinguishability
convertState :: [[String]] -> String -> String
convertState (c:cs) s = if s `elem` c
                        then head c
                        else convertState cs s

-- Convert rule according to classes
convertRule :: [[String]] -> Rule -> Rule
convertRule classes Rule {start = s, symbol = s_, end = e} =
    Rule (convertState classes s) s_ (convertState classes e)

-- Calculate classes of indistinguishablity
getClasses :: [[String]] -> [Rule] -> [[String]]
getClasses kindi r =
    if normalizeClass wheer == normalizeClass kindi
    then wheer
    else getClasses wheer r
    where
        wheer = concatMap (kIndis r kindi) kindi

-- Order classes by length and alphabetically
normalizeClass :: [[String]] -> [[String]]
normalizeClass [] = []
normalizeClass c = sortBy (comparing inteligentHead) $ map sort c

inteligentHead :: [String] -> String
inteligentHead [] = ""
inteligentHead (a:ss) = a

-- Calculate k+1 indistinguishability
kIndis :: [Rule] -> [[String]] -> [String] -> [[String]]
kIndis r cls cl =
    splitToClasses [] [] $ zip cl (map (findSecondStates r) cl)
    where
        findSecondStates r s = filter (/= "") $ map (getSecondState s) r
        getSecondState so Rule {start = s, symbol = s_, end = e} = if s == so
                                                                   then show $ getClassIndex 0 cls e
                                                                   else ""
        getClassIndex n (x:xs) e = if e `elem` x
                                   then n
                                   else getClassIndex (n+1) xs e

        splitToClasses m1 m2 [] = m2
        splitToClasses m1 m2 (w:ws) = if snd w `elem` m1
                                      then uncurry (foundAndContinue m1 m2 ws) w
                                      else splitToClasses (snd w:m1) ([fst w]:m2) ws

        foundAndContinue m1 m2 ws c w =
            splitToClasses m1 (take i m2 ++ [c:(m2 !! i)] ++ drop (i + 1) m2) ws
            where
                i = getIndex m1 w 0


        getIndex (x:xs) e n = if x == e
                              then n
                              else getIndex xs e (n+1)

completeA :: DFA -> DFA
completeA DFA {states = s, alphabet = a, rules = r, initState = i, finalStates = f} =
    if length r /= length s * length a
    then completeA_ $ DFA s a r i f
    else DFA s a (sortBy (comparing start) (sortBy (comparing symbol)r)) i f

-- Create new sink state and add transitions to this state for all not defined transitions
completeA_ :: DFA -> DFA
completeA_ DFA {states = s, alphabet = a, rules = r, initState = i, finalStates = f} =
    DFA (newState:s) a (sortBy (comparing start) (sortBy (comparing symbol)(r ++ newRules))) i f
    where
        newState = maximumBy (comparing length) s ++ "1"
        newRules = map (compR newState) $ [[x,y] | x <- newState:s, y <- a] \\ map getFS r
        getFS Rule { start = s, symbol = s_, end = e} = [s, s_]
        compR newState [f, s] = Rule f s newState
