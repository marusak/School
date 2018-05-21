/*
 * Spanning tree
 *
 * Find all spanning trees out of undirected graphs
 *
 * Author: Matej Marusak, xmarus06@stud.fit.vutbr.cz
 * Created as school project, BUT FIT, April 2018
 */

merge([],L,L).
merge([H|T], L, [H|M]) :- merge(T,L,M).

is_empty([]) :- true.
is_empty([_|_]) :- false.

removeHead([],[]).
removeHead([_|T], T).

normalize([], []).
normalize([First,Second|_], V):-merge(First, Second, R), sort(R, V).

has_cycle([], _) :- !,false.
has_cycle([[First,Second|_]|R], V) :- (has_cycle(First, [[First,Second]|R], V, []) ->
                                       !,true;
                                       has_cycle(R,[])).
has_cycle(_, [], _, _) :- !,false.
has_cycle(X, Unused, Visited, Tmp) :- member([X,Y|_], Unused),
                                      delete(Unused, [X,Y], NewUnused),
                                      (member(Y,Tmp) ; member(Y,Visited) ->
                                          !,true;
                                          has_cycle(X, NewUnused, Visited, [Y|Tmp])
                                      ).
has_cycle(X, Unused, Visited, Tmp) :- member([Y,X|_], Unused),
                                      delete(Unused, [Y,X], NewUnused),
                                      (member(Y,Tmp) ; member(Y,Visited) ->
                                          !,true;
                                          has_cycle(X, NewUnused, Visited, [Y|Tmp])
                                      ).
has_cycle(_, _, _, []) :- !,false.
has_cycle(X, Unused, Visited, [H|T]) :- has_cycle(H, Unused, [X|Visited], T).

find_span(T, [], T1, N) :- length(T,Y),
                           (N == Y ->
                               sort(T,T1);
                               T1=[]),
                            !.
find_span(T, [_|Rest], T1, N) :- find_span(T, Rest, T1, N).
find_span(T, [H|Rest], T1, N) :- length(H, C), (C \= 1, \+ has_cycle([H|T], []) ->
                                    find_span([H|T], Rest, T1, N);
                                    find_span(T, Rest, T1, N)).

printResults([]).
printResults([H|T]) :- (is_empty(H) -> true; printResult(H)), printResults(T).

printResult([]) :- nl.
printResult([H|T]) :- (is_empty(T) ->
                            format('~w-~w', H);
                            format('~w-~w ', H)),
                      printResult(T).

is_valid(X) :- length(X, N), (N == 2 -> true;false).

% FROM THIS POINT FORWARDS code is taken from official school template
%Reads line from stdin, terminates on LF or EOF.
read_line(L,C) :-
    get_char(C),
    (isEOFEOL(C), L = [], !;
        read_line(LL,_),% atom_codes(C,[Cd]),
        [C|LL] = L).

%Tests if character is EOF or LF.
isEOFEOL(C) :-
    C == end_of_file;
    (char_code(C,Code), Code==10).

read_lines(Ls) :-
    read_line(L,C),
    ( C == end_of_file, Ls = [] ;
      read_lines(LLs), Ls = [L|LLs]
    ).

% rozdeli radek na podseznamy
split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1).    % aby to fungovalo i s retezcem na miste seznamu
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]). % G je prvni seznam ze seznamu seznamu G|S1

% vstupem je seznam radku (kazdy radek je seznam znaku)
split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).

start :-
    prompt(_, ''),
    read_lines(LL),
    split_lines(LL,F),
    include(is_valid, F, S2),
    maplist(normalize, S2, S1),
    sort(S1,S),
    flatten(S, Flat),
    sort(Flat, Vertices),
    removeHead(Vertices, V1),
    length(V1, N),
    setof(R, find_span([], S, R, N), C),
    printResults(C),
    halt.
