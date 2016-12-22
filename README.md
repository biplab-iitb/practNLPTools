practNLPTools
=============

Practical Natural Language Processing Tools for Humans.<br>
practNLPTools is a pythonic library over SENNA and Stanford Dependency Extractor.

Functionality
=============
1. Semantic Role Labeling
2. Syntactic Parsing
3. Part of Speech Tagging (POS Tagging)
4. Named Entity Recognisation (NER)
5. Dependency Parsing
6. Shallow Chunking

Features
=============
1. Fast: SENNA is written is C. So it is Fast.
2. We use only dependency Extractor Component of Stanford Parser, which takes in Syntactic Parse from SENNA and applies dependency Extraction. So there is no need to load parsing models for Stanford Parser, which takes time.
3. Easy to use.
4. Platform Supported - Windows, Linux and Mac

Installation
=============

Requires:
A computer with 500mb memory, Java Runtime Environment (1.7 preferably, works with 1.6 too, but didnt test.) installed and python.

If you are in linux:
run:

    sudo python setup.py install 

If you are in windows:
run this commands as administrator:

    python setup.py install
  

Examples
=============

Chunk and NER use BIOS Tagging Scheme. Which expands to:

1. S = Tag covers Single Word.
2. B = Tag Begins with the Word.
3. I = Word is internal to tag which has begun.
4. E = Tag Ends with the Word.
5. 0 = Other tags.

Example:
  ('Biplab', 'S-NP'), ('is', 'S-VP'), ('a', 'B-NP'), ('good', 'I-NP'), ('boy', 'E-NP'), ('.', 'O')
  
  means:
  
  [Biplab]NP [is]VP [a good boy]NP [.]O
  

Annotator is the only class you need. Create an annotator object.

    >>>from practnlptools.tools import Annotator
    >>>annotator=Annotator()


Using Function getAnnoations(sentence) returns a dictionary of annotations.

    >>>annotator.getAnnotations("There are people dying make this world a better place for you and for me.")
    {'dep_parse': '', 'chunk': [('There', 'S-NP'), ('are', 'S-VP'), ('people', 'S-NP'), ('dying', 'B-VP'), ('make', 'E-VP'), ('this', 'B-NP'), ('world', 'E-NP'), ('a', 'B-NP'), ('better', 'I-NP'), ('place', 'E-NP'), ('for', 'S-PP'), ('you', 'S-NP'), ('and', 'O'), ('for', 'S-PP'), ('me.', 'S-NP')], 'pos': [('There', 'EX'), ('are', 'VBP'), ('people', 'NNS'), ('dying', 'VBG'), ('make', 'VB'), ('this', 'DT'), ('world', 'NN'), ('a', 'DT'), ('better', 'JJR'), ('place', 'NN'), ('for', 'IN'), ('you', 'PRP'), ('and', 'CC'), ('for', 'IN'), ('me.', '.')], 'srl': [{'A1': 'people', 'V': 'dying'}, {'A1': 'people  this world', 'A2': 'a better place for you and for me.', 'V': 'make'}], 'syntax_tree': '(S1(S(NP(EX There))(VP(VBP are)(NP(NP(NNS people))(SBAR(S(VBG dying)(VP(VB make)(S(NP(DT this)(NN world))(NP(DT a)(JJR better)(NN place)))(PP(PP(IN for)(NP(PRP you)))(CC and)(PP(IN for)(NP(. me.)))))))))))', 'verbs': ['dying', 'make'], 'words': ['There', 'are', 'people', 'dying', 'make', 'this', 'world', 'a', 'better', 'place', 'for', 'you', 'and', 'for', 'me.'], 'ner': [('There', 'O'), ('are', 'O'), ('people', 'O'), ('dying', 'O'), ('make', 'O'), ('this', 'O'), ('world', 'O'), ('a', 'O'), ('better', 'O'), ('place', 'O'), ('for', 'O'), ('you', 'O'), ('and', 'O'), ('for', 'O'), ('me.', 'O')]}


Using Function getAnnoations(sentence,dep_parse=True) returns a dictionary of annotations with dependency parse, by default it is switched off.

    >>>annotator.getAnnotations("There are people dying make this world a better place for you and for me.",dep_parse=True)
    {'dep_parse': 'expl(are-2, There-1)\nroot(ROOT-0, are-2)\nnsubj(are-2, people-3)\ndep(make-5, dying-4)\nrcmod(people-3, make-5)\ndet(world-7, this-6)\nnsubj(place-10, world-7)\ndet(place-10, a-8)\namod(place-10, better-9)\nxcomp(make-5, place-10)\nprep_for(make-5, you-12)\nconj_and(you-12, me.-15)', 'chunk': [('There', 'S-NP'), ('are', 'S-VP'), ('people', 'S-NP'), ('dying', 'B-VP'), ('make', 'E-VP'), ('this', 'B-NP'), ('world', 'E-NP'), ('a', 'B-NP'), ('better', 'I-NP'), ('place', 'E-NP'), ('for', 'S-PP'), ('you', 'S-NP'), ('and', 'O'), ('for', 'S-PP'), ('me.', 'S-NP')], 'pos': [('There', 'EX'), ('are', 'VBP'), ('people', 'NNS'), ('dying', 'VBG'), ('make', 'VB'), ('this', 'DT'), ('world', 'NN'), ('a', 'DT'), ('better', 'JJR'), ('place', 'NN'), ('for', 'IN'), ('you', 'PRP'), ('and', 'CC'), ('for', 'IN'), ('me.', '.')], 'srl': [{'A1': 'people', 'V': 'dying'}, {'A1': 'people  this world', 'A2': 'a better place for you and for me.', 'V': 'make'}], 'syntax_tree': '(S1(S(NP(EX There))(VP(VBP are)(NP(NP(NNS people))(SBAR(S(VBG dying)(VP(VB make)(S(NP(DT this)(NN world))(NP(DT a)(JJR better)(NN place)))(PP(PP(IN for)(NP(PRP you)))(CC and)(PP(IN for)(NP(. me.)))))))))))', 'verbs': ['dying', 'make'], 'words': ['There', 'are', 'people', 'dying', 'make', 'this', 'world', 'a', 'better', 'place', 'for', 'you', 'and', 'for', 'me.'], 'ner': [('There', 'O'), ('are', 'O'), ('people', 'O'), ('dying', 'O'), ('make', 'O'), ('this', 'O'), ('world', 'O'), ('a', 'O'), ('better', 'O'), ('place', 'O'), ('for', 'O'), ('you', 'O'), ('and', 'O'), ('for', 'O'), ('me.', 'O')]}

You can access individual componets as:

    >>>annotator.getAnnotations("Biplab is a good boy.")['pos']
    [('Biplab', 'NNP'), ('is', 'VBZ'), ('a', 'DT'), ('good', 'JJ'), ('boy', 'NN'), ('.', '.')]
    >>>annotator.getAnnotations("Biplab is a good boy.")['ner']
    [('Biplab', 'S-PER'), ('is', 'O'), ('a', 'O'), ('good', 'O'), ('boy', 'O'), ('.', 'O')]
    >>>annotator.getAnnotations("Biplab is a good boy.")['chunk']
    [('Biplab', 'S-NP'), ('is', 'S-VP'), ('a', 'B-NP'), ('good', 'I-NP'), ('boy', 'E-NP'), ('.', 'O')]


To list the verbs for which semantic roles are found.

    >>>annotator.getAnnotations("He created the robot and broke it after making it.")['verbs']
    ['created', 'broke', 'making']

'srl' Returns a list of dictionaries, identifyinging sematic roles for various verbs in sentence.

    >>>annotator.getAnnotations("He created the robot and broke it after making it.")['srl']
    [{'A1': 'the robot', 'A0': 'He', 'V': 'created'}, {'A1': 'it', 'A0': 'He', 'AM-TMP': 'after making it.', 'V': 'broke'}, {'A1': 'it.', 'A0': 'He', 'V': 'making'}]

'syntax_tree' Returns syntax tree in penn Tree Bank Format.

    >>>annotator.getAnnotations("He created the robot and broke it after making it.")['syntax_tree']
    '(S1(S(NP(PRP He))(VP(VP(VBD created)(NP(DT the)(NN robot)))(CC and)(VP(VBD broke)(NP(PRP it))(PP(IN after)(S(VP(VBG making)(NP(PRP it.)))))))))'

'dep_parse' Returns dependency Relations as a string. Each relation is in new line. You may require some post processing on this.

    >>>print annotator.getAnnotations("He created the robot and broke it after making it.",dep_parse=True)['dep_parse']
    nsubj(created-2, He-1)
    root(ROOT-0, created-2)
    det(robot-4, the-3)
    dobj(created-2, robot-4)
    conj_and(created-2, broke-6)
    dobj(broke-6, it-7)
    prepc_after(broke-6, making-9)
    dobj(making-9, it.-10)


If there are many sentences to annotate, Use batch Mode, annotator.getBatchAnnotations(sentences,dep_parse=True/False). Returns a list of annotation dictionaries.

    >>>annotator.getBatchAnnotations(["He created the robot and broke it after making it.","Biplab is a good boy."],dep_parse=True)
    [{'dep_parse': 'nsubj(created-2, He-1)\nroot(ROOT-0, created-2)\ndet(robot-4, the-3)\ndobj(created-2, robot-4)\nconj_and(created-2, broke-6)\ndobj(broke-6, it-7)\nprepc_after(broke-6, making-9)\ndobj(making-9, it.-10)', 'chunk': [('He', 'S-NP'), ('created', 'S-VP'), ('the', 'B-NP'), ('robot', 'E-NP'), ('and', 'O'), ('broke', 'S-VP'), ('it', 'S-NP'), ('after', 'S-PP'), ('making', 'S-VP'), ('it.', 'S-NP')], 'pos': [('He', 'PRP'), ('created', 'VBD'), ('the', 'DT'), ('robot', 'NN'), ('and', 'CC'), ('broke', 'VBD'), ('it', 'PRP'), ('after', 'IN'), ('making', 'VBG'), ('it.', 'PRP')], 'srl': [{'A1': 'the robot', 'A0': 'He', 'V': 'created'}, {'A1': 'it', 'A0': 'He', 'AM-TMP': 'after making it.', 'V': 'broke'}, {'A1': 'it.', 'A0': 'He', 'V': 'making'}], 'syntax_tree': '(S1(S(NP(PRP He))(VP(VP(VBD created)(NP(DT the)(NN robot)))(CC and)(VP(VBD broke)(NP(PRP it))(PP(IN after)(S(VP(VBG making)(NP(PRP it.)))))))))', 'verbs': ['created', 'broke', 'making'], 'words': ['He', 'created', 'the', 'robot', 'and', 'broke', 'it', 'after', 'making', 'it.'], 'ner': [('He', 'O'), ('created', 'O'), ('the', 'O'), ('robot', 'O'), ('and', 'O'), ('broke', 'O'), ('it', 'O'), ('after', 'O'), ('making', 'O'), ('it.', 'O')]}, {'dep_parse': 'nsubj(boy-5, Biplab-1)\ncop(boy-5, is-2)\ndet(boy-5, a-3)\namod(boy-5, good-4)\nroot(ROOT-0, boy-5)', 'chunk': [('Biplab', 'S-NP'), ('is', 'S-VP'), ('a', 'B-NP'), ('good', 'I-NP'), ('boy', 'E-NP'), ('.', 'O')], 'pos': [('Biplab', 'NNP'), ('is', 'VBZ'), ('a', 'DT'), ('good', 'JJ'), ('boy', 'NN'), ('.', '.')], 'srl': [], 'syntax_tree': '(S1(S(NP(NNP Biplab))(VP(VBZ is)(NP(DT a)(JJ good)(NN boy)))(. .)))', 'verbs': [], 'words': ['Biplab', 'is', 'a', 'good', 'boy', '.'], 'ner': [('Biplab', 'S-PER'), ('is', 'O'), ('a', 'O'), ('good', 'O'), ('boy', 'O'), ('.', 'O')]}]


Note: For illustration purposes we have used:

    >>>annotator.getAnnotations("He created the robot and broke it after making it.",dep_parse=True)['dep_parse']

Better method is:

    >>>annotation=annotator.getAnnotations("He created the robot and broke it after making it.",dep_parse=True)
    >>>ner=annotation['ner']
    >>>srl=annotation['srl']

Issues
=============

1. You cannot give sentence with '(' ')', that is left bracket aor right bracket. It will end up in returning no result. So please clean Sentences before sending to annotator.
2. Other issue might be senna executable built for various platforms. I have not experienced it, but its highly probable. If you get this issuse:

Go to folder practnlptools

    cd practnlptools
    gcc -O3 -o senna-linux64 *.c  (For linux 64 bit)
    gcc -O3 -o senna-linux32 *.c  (For linux 32 bit)
    gcc -O3 -o senna-senna-osx *.c (For Mac)
    *windows: I never compiled C files in Windows.*
    python setup.py install

3. Any other, you can la la laa la laaaa to  biplab12  (A T) cse d0t iitb d0t ac d0t in 
4. Issues with "pip install practnlptools"
   
You might receive following Error while running:
<pre>  
 Traceback (most recent call last):
 File "test.py", line 3, in <module>
    print a.getAnnotations("This is a test.")
  File "/usr/local/lib/python2.7/dist-packages/practnlptools/tools.py", line 206, in getAnnotations
    senna_tags=self.getSennaTag(sentence)
  File "/usr/local/lib/python2.7/dist-packages/practnlptools/tools.py", line 88, in getSennaTag
    p = subprocess.Popen(senna_executable,stdout=subprocess.PIPE, stdin=subprocess.PIPE)
  File "/usr/lib/python2.7/subprocess.py", line 679, in __init__
    errread, errwrite)
  File "/usr/lib/python2.7/subprocess.py", line 1249, in _execute_child
    raise child_exception
OSError: [Errno 13] Permission denied
</pre>
To Fix this,you can do:
  chmod -R +x /usr/local/lib/python2.7/dist-packages/practnlptools/
   
