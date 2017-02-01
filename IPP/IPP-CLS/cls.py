# !/usr/bin/python3
# coding=utf-8
#CLS:xmarus06
"""Analyser of inheritance in C++ classes.

Author: Matej Marusak, VUT FIT 2016, IPP-CLS proj. 2

    The one and only file that was needed.
"""
from __future__ import print_function
import sys
import re
from lxml.etree import Element, SubElement, tostring
from xml.dom import minidom


def error(message, error_code):
    """Print exit message to stderr and exit."""
    print(message, file=sys.stderr)
    sys.exit(error_code)


def help():
    """Print help and exit."""
    print ("\033[34m                    C++ Classes\033[0m \n")
    print ("Input is a valid C++ header file.\n")
    print ("Input is analysed and XML with classes hierarchy is created.\n")
    print ("\nAuthor: Matej Marusak, xmarus06@stud.fit.vutbr.cz\n")
    print ("As a school project for BUT FIT, Czech Republic, Subject :IPP\n")
    print ("Created: April 2016\n\n\n")
    print ("Usage: python3 cls.py --help\n")
    print ("       python3 cls.py [--input=filename] [--output=filename] \
[--pretty-xml[=k] [--details[=class] [--search=XPATH]\n")
    print ("\nOptions:\n")
    print ("     --help: Print this help and exit\n")
    print ("     --input=filename: Input file in UTF-8. \
If missing, stdin is considered.\n")
    print ("     --output=filename: Output file in UTF-8. \
If missing, stdout is considered.\n")
    print ("     --pretty-xml: Print xml well formated with intend k spaces \
(if k not specified, 4 is conisdered.\n")
    print ("     --details=class: Output are details of class specified, \
if class missing, all classes are printed\n")
    print ("     --search=XPATH: Output is result of XPATH\n\n\n")
    exit(0)


def prettify(elem, n):
    """Return a pretty-printed XML string - from pymotw.com."""
    if (elem == []):
        return('<?xml version="1.0" encoding="utf-8"?>\n')
    if (isinstance(elem, str)):
        return (" "*n+elem)
    rough_string = tostring(elem)
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent=" "*n)


def parseCommandLine(cmd_line):
    """Parse command line.

    cmd_line: list of arguments without script name
    return :
    """
    result = {}
    for arg in cmd_line:
        matches = re.search("^--((\w|-)*)(=([^\s]+))?$", arg)
        if (matches is None):
            error("Dude, thats not even argument", 1)
        if (matches.group(1) in result.keys()):
            error("Common, I already got it, no need to tell me twice", 1)
        if (matches.group(1) not in ("help", "input", "output", "pretty-xml",
           "details", "search", "conflicts")):
            error("Holy moly, what was that?!", 1)
        if ((matches.group(4) == "" or matches.group(4) is None) and
            matches.group(1) != "details" and
            matches.group(1) != "pretty-xml" and
           matches.group(1) != 'help' and
           matches.group(1) != 'conflicts'):
            error("I/O, details and search need som additional info", 1)
        if (matches.group(1) == 'help' and matches.group(4) is not None):
            error("Help does not take any arguments", 1)
        result[matches.group(1)] = matches.group(4)
    if ('conflicts' in result and 'details' not in result):
        error("Conflicts without detials", 1)
    return result


def getToken(acc):
    """Return one token."""
    result = re.match("\s*(\w+|\{|\}|\(|\)|;|\=|,|:|\*|&|~)\s*([\s\S]*)", acc)
    if (result is None):
        error("Token nepozna typ "+acc+"\n", 69)
    if (result.group(1) == ":" and result.group(2)[0] == ":"):  # ::
        return ("::", result.group(2)[1:])
    else:
        return (result.group(1), result.group(2))


def getType(token, cls):
    """Return type of variable or function."""
    acc_type = ""
    while True:  # simulation do-while
        acc_type = acc_type + token + " "
        token, cls = getToken(cls)
        if (token not in
            ("void", "bool", "char", "int", "float", "double", "void",
             "wchar_t", "signed", "unsigned", "short", "long", "*", "&")):
            return (acc_type[:-1], token+" "+cls)


def getFArgs(cls):
    """Parse function arguements."""
    f_arg_types = ()
    f_arg_names = ()
    token = ""
    while (token != ")"):
        token, cls = getToken(cls)
        if (token == ")"):
            return (cls, f_arg_names, f_arg_types)
        arg, cls = getType(token, cls)  # type
        if (arg == "void"):
            name, cls = getToken(cls)  # )
            return (cls, f_arg_names, f_arg_types)
        token, cls = getToken(cls)  # name
        if token not in (',', ')'):
            name = token
            token, cls = getToken(cls)  # comma or )
        else:
            name = ""
        f_arg_types = f_arg_types + (arg,)
        f_arg_names = f_arg_names + (name,)

    return (cls, f_arg_names, f_arg_types)


def parseClasses(cls):
    """Berie cely vstup, rozprasuje ho na triedy a vlozi to do struktury.

    Returns list of classes, where each class consists from:
    key = name
    [0] = list of parents (ClassName, type)
    [1] = methods ((Name, arguments_types) return_type, argumenst_names
          def/decl, virtual, pureVirtual, privacy, static, from, printable)
    [2] = instances((Name), type, def/decl, virtual, privacy,
                           static,from, printable)
    [3] = usings (name) from, privacy
    [4] = conflicts
    """
    classes = {}
    while (cls != ""):
        token, cls = getToken(cls)
        if (token != "class"):
            error("Musi byt class", 4)
        # name
        className, cls = getToken(cls)
        token, cls = getToken(cls)
        parents = {}
        methods = {}
        instances = {}
        using = {}
        while (token != "{" and token != ";"):
            token, cls = getToken(cls)  # comma, no control
            # read the inheritance
            if (token in ("private", "protected", "public")):
                papaName, cls = getToken(cls)
                if papaName not in classes.keys():
                    error("Neda sa dedit z triedy, ktora neexistuje", 4)
                if classes[papaName] == 'declared':
                    error("Neda sa dedit z triedy, kt. je len deklarovana", 4)
                if (papaName in parents.keys()):
                    error("Duplicate base class", 4)
                parents[papaName] = token
            else:
                if token not in classes.keys():
                    error("Neda sa dedit z triedy, ktora neexistuje", 4)
                if classes[token] == 'declared':
                    error("Neda sa dedit z triedy, kt. je len deklarovana", 4)
                if (token in parents.keys()):
                    error("Duplicate base class", 4)
                parents[token] = "private"
            token, cls = getToken(cls)

        # declaration -> moze sposobit problemy len v takejto forme
        if (token == ";"):  # only class declaration
            if (className not in classes.keys()):
                classes[className] = "declared"
                continue
            else:
                continue

        # implicitly private
        privacy = "private"
        token, cls = getToken(cls)
        # till the end of actuall class
        # one loop = one method or instance or privacy modifier
        while (token != "}"):
            virtual = False
            static = False
            if (token == "virtual"):
                virtual = True
                if (static):
                    error("Static and virtual!", 4)
                token, cls = getToken(cls)
            if (token == "static"):
                static = True
                if (virtual):
                    error("Static and virtual!", 4)
                token, cls = getToken(cls)
            if (token == "virtual"):
                virtual = True
                if (static):
                    error("Static and virtual!", 4)
                token, cls = getToken(cls)
            if (token in ("private", "protected", "public")):
                privacy = token
                token, cls = getToken(cls)  # :, no control
                token, cls = getToken(cls)
                continue
            if (token == "using"):
                fromName, cls = getToken(cls)
                if (fromName not in parents.keys()):
                    error("Neda sa using z triedy z ktorej sa nededi", 4)
                token, cls = getToken(cls)  # ::
                whatName, cls = getToken(cls)
                token, cls = getToken(cls)  # ;
                if (token != ";"):
                    error("Nespravna praca s using vramci kontextu IPP-CLS", 4)
                using[whatName] = (fromName, privacy)
                token, cls = getToken(cls)
                continue
            if (token == "~"):  # Destructor
                name, cls = getToken(cls)
                if (name != className):
                    error("Destructor but with wrong name", 4)
                name = "~"+name
                token, cls = getToken(cls)
                token, cls = getToken(cls)
                if (token != ")"):
                    error("Destructor does not have any arguments", 4)
                token, cls = getToken(cls)
                if (token == ";"):
                    if ((name, ()) in methods.keys()):
                        error("Overloading of destructor", 4)
                    methods[name, ()] = ["void", (), "declared", virtual,
                                         False, privacy, static,
                                         className, True]
                    token, cls = getToken(cls)
                    continue
                token, cls = getToken(cls)  # }
                if (token != "}"):
                    print (token)
                    print (cls)
                    error("What was that, destructor stuff", 4)
                token, cls = getToken(cls)
                if (token == ";"):
                    token, cls = getToken(cls)
                if ((name, ()) in methods.keys()):
                    error("Redefinicia destuktoru", 4)
                methods[name, ()] = ["void", (), "defined", virtual,
                                     False, privacy, static, className, True]
                continue
            if (token == className):  # Contructor
                name = token
                token, cls = getToken(cls)
                f_arg_types = ()
                f_arg_names = ()
                cls, f_arg_names, f_arg_types = getFArgs(cls)
                token, cls = getToken(cls)
                if (token == ";"):  # constructor declaration
                    if ((name, f_arg_types) in methods.keys()):
                        error("Overloading of constructor", 4)
                    methods[name, f_arg_types] = [name, f_arg_names,
                                                  "declared", virtual,
                                                  False, privacy, static,
                                                  className, True]
                    token, cls = getToken(cls)
                    continue
                token, cls = getToken(cls)  # {
                if (token != "}"):
                    error("What was that, constructor stuff", 4)
                token, cls = getToken(cls)
                if (token == ";"):
                    token, cls = getToken(cls)
                if ((name, f_arg_types) in methods.keys()):
                    error("Redefinicia constructoru", 4)
                methods[name, f_arg_types] = [name, f_arg_names, "defined",
                                              virtual, False, privacy,
                                              static, className, True]
                continue
            acc_type, cls = getType(token, cls)  # get type
            token, cls = getToken(cls)  # get names
            acc_name = token
            token, cls = getToken(cls)
            if (token == ";"):  # instance declaration
                if (acc_name in instances.keys()):
                    error("Overloading of instance", 4)
                if (acc_name in [c[0] for c in methods.keys()]):
                    error("Variable name cannot be a method name as well", 4)
                instances[acc_name] = [acc_type, "declared", virtual,
                                       privacy, static, className, True]
                token, cls = getToken(cls)
                continue
            if (token == "="):  # instance definition
                while (token != ";"):
                    token, cls = getToken(cls)
                if (acc_name in instances.keys()):
                    error("Redefinicia instancie", 4)
                if (acc_name in [c[0] for c in methods.keys()]):
                    error("Variable name cannot be a method name as well", 4)
                instances[acc_name] = [acc_type, "defined", virtual, privacy,
                                       static, className, True]
                token, cls = getToken(cls)
                continue
            if (token != "("):
                error("I dont know the input character1 "+token+" "+cls, 4)
            # function declaration/definition
            # read arguments
            # token, cls = getToken(cls)
            f_arg_types = ()
            f_arg_names = ()
            cls, f_arg_names, f_arg_types = getFArgs(cls)
            token, cls = getToken(cls)
            if (token == ";"):  # method declaration
                if ((acc_name, f_arg_types) in methods.keys()):
                    error("Redeklaracia metody", 4)
                if (acc_name in instances.keys()):
                    error("Variable name cannot be a method name as well", 4)
                methods[acc_name, f_arg_types] = [acc_type, f_arg_names,
                                                  "declared", virtual, False,
                                                  privacy, static, className,
                                                  True]
                token, cls = getToken(cls)
                continue
            if (token == "="):  # pure virtual
                token, cls = getToken(cls)  # 0 no control
                token, cls = getToken(cls)  # no control
                if (not virtual):
                    error("Pure cirtual ale nie virtual", 4)
                if ((acc_name, f_arg_types) in methods.keys()):
                    error("Redeklaracia metody", 4)
                if (acc_name in instances.keys()):
                    error("Variable name cannot be a method name as well", 4)
                methods[acc_name, f_arg_types] = [acc_type, f_arg_names,
                                                  "declared", virtual, True,
                                                  privacy, static, className,
                                                  True]
                token, cls = getToken(cls)
                continue
            if (token != "{"):
                error("I dont know the input character1 "+token+" "+cls, 4)
            token, cls = getToken(cls)  # }
            if ((acc_name, f_arg_types) in methods.keys()):
                error("Redefinicia metody", 4)
            if (acc_name in instances.keys()):
                error("Variable name cannot be a method name as well", 4)
            methods[acc_name, f_arg_types] = [acc_type, f_arg_names, "defined",
                                              virtual, False, privacy, static,
                                              className, True]
            token, cls = getToken(cls)  # ;
            if (token == ";"):
                token, cls = getToken(cls)  # }
        token, cls = getToken(cls)  # ; or another class
        if (className in classes.keys()):  # uz mame danu triedu vnutri
            if (classes[className] == "declared"):
                classes[className] = [parents, methods, instances, using, []]
            else:
                error("redefinicia triedy", 4)
        else:
            classes[className] = [parents, methods, instances, using, []]

    return classes


def editMethod(fromP, m_t, m, to, toWho, conflicts):
    """edit function in relation to the context.

    fromP - base class + privacy
    m_t - method name
    m - method we want to add
    to - all methods that are already in
    toWho - name of claas to which we want to add
    @return: (true ,method) or (false,..) if nothing to be done
    """
    printable = m[8]
    if (m[5] == 'private' and not m[4]):  # private, no need to do anything
        printable = False
    privacy = m[5]
    if (fromP[1] != 'public'):
        privacy = fromP[1]
    if (m_t[0] == fromP[0] or m_t[0] == '~'+fromP[0]):
        return (False, [])
    if m_t not in to.keys() and m_t[0] not in conflicts.keys():
        if (m[4]):  # pure virtual
            return (True, [m[0], m[1], m[2], m[3], m[4],
                    m[5], m[6], m[7], printable])
        else:
            return (True, [m[0], m[1], m[2], m[3], m[4],
                    privacy, m[6], m[7], printable])
    elif m_t[0] in conflicts.keys():
        if (fromP[0] == conflicts[m_t[0]][0]):
            return (True, [m[0], m[1], m[2], m[3], m[4], conflicts[m_t[0]][1],
                    m[6], m[7], printable])
        else:
            return (False, [])
    else:  # already in and not specified by conflict
        if (to[m_t][7] == toWho):
            return (False, [])
        else:
            return (False, [m_t[0],
                    ['method',
                    [m[0], m[1], m[2], m[3], m[4], privacy, m[6], m[7], True],
                    m_t],
                    ['method',
                    [to[m_t][0], to[m_t][1], to[m_t][2], to[m_t][3], to[m_t][4],
                     to[m_t][5], to[m_t][6], to[m_t][7], True], m_t]
                    ])


def editInstance(fromP, i_t, i, to, toWho, conflicts):
    """edit instance in relation to the context.

    fromP - base class + privacy
    i_t - instnace name
    i - instance we want to add
    to - all instances that are already in
    toWho - name of claas to which we want to add
    @return: (true ,method) or (false,..) if nothing to be done
    """
    printable = i[6]
    if (i[3] == 'private'):  # when private, no need to do anything
        printable = False
    privacy = i[3]
    if (fromP[1] != 'public'):
        privacy = fromP[1]
    if i_t not in to.keys() and i_t not in conflicts.keys():
        return (True, [i[0], i[1], i[2], privacy, i[4],
                i[5], printable])
    elif i_t in conflicts.keys():
        if (fromP[0] == conflicts[i_t][0]):
            return (True, [i[0], i[1], i[2], conflicts[i_t][1], i[4],
                    i[5], printable])
        else:
            return (False, [])
    else:  # already in
        if (to[i_t][5] == toWho):
            return (False, [])
        else:
            return (False, [i_t,
                    ['instance', [i[0], i[1], i[2], privacy, i[4], i[5], True]],
                     ['instance', [to[i_t][0], to[i_t][1], to[i_t][2], to[i_t][3],
                      to[i_t][4], to[i_t][5], True]]
                    ])


def makeClassesComplete(cs):
    """Fill inhereted classes methods and instances.

    cs: output from parseClasses
    """
    opened = []  # classes, that needs to be solved
    closed = []  # classes, that are solved solved
    for key in cs.keys():
        if (cs[key][0] == {}):  # no parents
            closed.append(key)
            continue
        else:
            opened.append(key)
    while (opened != []):
        for item in opened:
            if (not[e for e in cs[item][0].keys() if e not in closed]):
                for par in cs[item][0].keys():
                    # spracuj metody
                    for mt in cs[par][1].keys():
                        toDo, newM = editMethod([par, cs[item][0][par]], mt,
                                                cs[par][1][mt], cs[item][1],
                                                item, cs[item][3])
                        if (toDo):
                            if (mt[0] in cs[item][2].keys()):
                                cs[item][4].append([mt[0],
                                                   ['method', newM, mt],
                                                   ['instance', cs[item][2][mt[0]]]])
                                del cs[item][2][mt[0]]
                            else:
                                cs[item][1][mt] = newM
                        elif (newM != []):
                            del cs[item][1][mt]
                            cs[item][4].append(newM)
                    # spracuj instancie
                    for ins in cs[par][2].keys():
                        toDo, newI = editInstance([par, cs[item][0][par]], ins,
                                                  cs[par][2][ins], cs[item][2],
                                                  item, cs[item][3])
                        if (toDo):
                            conf = [c for c in cs[item][1].keys() if c[0] == ins]
                            if conf:
                                cs[item][4].append([ins,
                                                   ['method', cs[item][1][conf[0]], conf[0]],
                                                    ['instance', newI]])
                                del cs[item][1][conf[0]]
                            else:
                                cs[item][2][ins] = newI
                        elif (newI != []):
                            del cs[item][2][ins]
                            cs[item][4].append(newI)
                    # pridaj vsetky konflikty
                    # for parCon in cs[par][4]:
                    #    cs[item][4].append(parCon)
                # add to solved classes (closed)
                closed.append(item)
                # remove from opened
                opened.remove(item)
    return cs


def getXMLHierarchy(acc, cs, top):
    """Return class hierarchy in XML.

    acc - class, that is now being examined
    cs - all classes
    top - the parent element
    """
    abstract = [c for c in cs[acc][1].keys() if cs[acc][1][c][4]]
    kind = 'abstract' if (abstract) else 'concrete'
    child = SubElement(top, 'class', {'kind': kind, 'name': acc})
    chls = [c for c in cs.keys() if acc in cs[c][0].keys()]
    if (chls):
        for cl in chls:
            getXMLHierarchy(cl, cs, child)


def makeXMLInstance(name, atts, top, fromWho):
    """Create XML for one instance.

    name - name of instance
    atts - attributes of instance
    top - parent element
    fromWho - name of class in which this instance is located
    """
    if (not atts[6]):
        return
    stat = 'static' if (atts[4]) else 'instance'
    i = SubElement(top, 'attribute',
                   {'name': name, 'type': atts[0], 'scope': stat})
    if (fromWho and atts[5] != fromWho[0]):
        SubElement(i, 'from', {'name': atts[5]})


def makeXMLMethod(name, atts, top, fromWho):
    """Create XML for one method.

    name - name of method
    atts - attributes of method
    top - parent element
    fromm - name of class in which this method is located
    """
    if (not atts[8]):
        return
    if (atts[6]):
        stat = 'static'
    else:
        stat = 'instance'
    m = SubElement(top, 'method',
                   {'name': name[0], 'type': atts[0], 'scope': stat})
    if (fromWho and atts[7] != fromWho[0]):
        SubElement(m, 'from', {'name': atts[7]})
    if (atts[3] or atts[4]):
        if atts[4]:
            pure = 'yes'
        else:
            pure = 'no'
        SubElement(m, 'virtual', {'pure': pure})
    args = SubElement(m, 'arguments')
    for arg in range(len(name[1])):
        SubElement(args, 'argument',
                   {'name': atts[1][arg], 'type': name[1][arg]})


def getXMLClassDetails(name, atts, t):
    """Create a xml detail of one class.

    name - name of className
    atts - all attributes of class (out from makeClassComplete or parseClasses)
    t - if not false, put new class on that
    @return - element of class acc
    """
    abstract = [c for c in atts[1].keys() if atts[1][c][4]]
    if (abstract):
        kind = 'abstract'
    else:
        kind = 'concrete'
    if (t is not False):
        top = SubElement(t, 'class', {'name': name, 'kind': kind})
    else:
        top = Element('class', {'name': name, 'kind': kind})

    # Inheritance
    if (atts[0]):
        inheritance = SubElement(top, 'inheritance')
        for base in atts[0].keys():
            SubElement(inheritance, 'from',
                       {'name': base, 'privacy': atts[0][base]})

    # find all:
    private_m = [c for c in atts[1].keys() if atts[1][c][5] == 'private' and atts[1][c][8]]
    public_m = [c for c in atts[1].keys() if atts[1][c][5] == 'public' and atts[1][c][8]]
    protected_m = [c for c in atts[1].keys() if atts[1][c][5] == 'protected' and atts[1][c][8]]

    private_i = [c for c in atts[2].keys() if atts[2][c][3] == 'private' and atts[2][c][6]]
    public_i = [c for c in atts[2].keys() if atts[2][c][3] == 'public' and atts[2][c][6]]
    protected_i = [c for c in atts[2].keys() if atts[2][c][3] == 'protected' and atts[2][c][6]]

    if (private_m or private_i):
        private = SubElement(top, 'private')
    if (public_m or public_i):
        public = SubElement(top, 'public')
    if (protected_m or protected_i):
        protected = SubElement(top, 'protected')

    if (private_m):
        priv_m = SubElement(private, 'methods')
        for m in private_m:
            makeXMLMethod(m, atts[1][m], priv_m, name)

    if (public_m):
        publ_m = SubElement(public, 'methods')
        for m in public_m:
            makeXMLMethod(m, atts[1][m], publ_m, name)

    if (protected_m):
        prot_m = SubElement(protected, 'methods')
        for m in protected_m:
            makeXMLMethod(m, atts[1][m], prot_m, name)

    if (private_i):
        priv_i = SubElement(private, 'attributes')
        for i in private_i:
            makeXMLInstance(i, atts[2][i], priv_i, name)

    if (public_i):
        publ_i = SubElement(public, 'attributes')
        for i in public_i:
            makeXMLInstance(i, atts[2][i], publ_i, name)

    if (protected_i):
        prot_i = SubElement(protected, 'attributes')
        for i in protected_i:
            makeXMLInstance(i, atts[2][i], prot_i, name)
    if (atts[4]):
        conflicts = SubElement(top, 'conflicts')
        for con in atts[4]:
            member = SubElement(conflicts, 'member', {'name': con[0]})
            from1 = con[1][1][5] if con[1][0] == 'instance' else con[1][1][7]
            from2 = con[2][1][5] if con[2][0] == 'instance' else con[2][1][7]
            privacy1 = con[1][1][3] if con[1][0] == 'instance' else con[1][1][5]
            privacy2 = con[2][1][3] if con[2][0] == 'instance' else con[2][1][5]
            if (from1 == from2):
                cl = SubElement(member, 'class', {'name': from1})
                if (privacy1 == privacy2):
                    priv = SubElement(cl, privacy1)
                    if con[1][0] == 'instance':
                        makeXMLInstance(con[0], con[1][1], priv, False)
                    else:
                        makeXMLMethod(con[1][2], con[1][1], priv, False)
                    # if con[2][0] == 'instance':
                    #    makeXMLInstance(con[0], con[2][1], priv, False)
                    # else:
                    #    makeXMLMethod(con[2][2], con[2][1], priv, False)
                else:
                    priv1 = SubElement(cl, privacy1)
                    priv2 = SubElement(cl, privacy2)
                    if con[1][0] == 'instance':
                        makeXMLInstance(con[0], con[1][1], priv1, False)
                    else:
                        makeXMLMethod(con[1][2], con[1][1], priv1, False)
                    if con[2][0] == 'instance':
                        makeXMLInstance(con[0], con[2][1], priv2, False)
                    else:
                        makeXMLMethod(con[2][2], con[2][1], priv2, False)
            else:
                cl1 = SubElement(member, 'class', {'name': from1})
                cl2 = SubElement(member, 'class', {'name': from2})
                priv1 = SubElement(cl1, privacy1)
                priv2 = SubElement(cl2, privacy2)
                if con[1][0] == 'instance':
                    makeXMLInstance(con[0], con[1][1], priv1, False)
                else:
                    makeXMLMethod(con[1][2], con[1][1], priv1, False)
                if con[2][0] == 'instance':
                    makeXMLInstance(con[0], con[2][1], priv2, False)
                else:
                    makeXMLMethod(con[2][2], con[2][1], priv2, False)
    return top


def main():
    """Main."""
    parsed = parseCommandLine(sys.argv[1:])
    keys = parsed.keys()
    if ("help" in keys):
        if (len(keys) != 1):
            error("Kombinovat --help a ine argumenty sa nepatri", 1)
        help()
    if ("input" not in keys):
        inputStream = sys.stdin
    else:
        try:
            inputStream = open(parsed["input"], "r")
        except:
            error("Input file is not valid or readable", 2)
    inputContent = inputStream.read()
    parsedClasses = parseClasses(inputContent)
    parsedClasses = makeClassesComplete(parsedClasses)
    if ('conflicts' not in parsed.keys() and
       [c for c in parsedClasses.keys() if parsedClasses[c][4]]):
        error("Conflict", 21)

    if ('pretty-xml' in parsed.keys()):
        pretty = 4 if not parsed['pretty-xml'] else int(parsed['pretty-xml'])
    else:
        pretty = 4

    if ('details' not in parsed.keys()):
        top = Element('model')
        base = [c for c in parsedClasses.keys() if parsedClasses[c][0] == {}]
        for b in base:
            getXMLHierarchy(b, parsedClasses, top)
    else:
        if (parsed['details']):
            top = []
            if (parsed['details'] in parsedClasses.keys()):
                top = getXMLClassDetails(parsed['details'],
                                         parsedClasses[parsed['details']], False)
        else:  # all the classes
            top = Element('model')
            for cl in parsedClasses.keys():
                getXMLClassDetails(cl, parsedClasses[cl], top)
        if ('search' in parsed.keys()):
            r = top.xpath(parsed['search'])
            top = Element('result')
            toWrite = ""
            if r:
                for item in sorted(r):
                    if (isinstance(item, str)):
                        toWrite += "\n" + pretty * " " + item
                    else:
                        top.append(item)
                top.text = toWrite+"\n"
            else:
                pass  # TODO len hlavicka alebo tak volako
    final = prettify(top, pretty)
    if ("output" not in keys):
        outputStream = sys.stdout
    else:
        try:
            outputStream = open(parsed["output"], "w")
        except:
            error("Output file is not valid or writable", 3)
    outputStream.write(final)
main()
