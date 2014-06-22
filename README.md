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

Installation
=============

If you are in linux:
  run:
    sudo python setup.py install 

If you are in windows:
  run this commands as administrator:
    sudo python setup.py install
  

Examples
=============
  <pre>

'''Annotator is the only class you need. Create an annotator object.'''
\>\>\>from practnlptools.tools import Annotator
\>\>\>fannotator=Annotator()

'''Using Function getAnnoations(sentence) returns a dictionary of annotations'''
\>\>\> annotator.getAnnotations("There are people dying make this world a better place for you and for me.",dep_parse=True)
{'dep_parse': 'expl(are-2, There-1)\nroot(ROOT-0, are-2)\nnsubj(are-2, people-3)\ndep(make-5, dying-4)\nrcmod(people-3, make-5)\ndet(world-7, this-6)\nnsubj(place-10, world-7)\ndet(place-10, a-8)\namod(place-10, better-9)\nxcomp(make-5, place-10)\nprep_for(make-5, you-12)\nconj_and(you-12, me.-15)', 'chunk': [('There', 'S-NP'), ('are', 'S-VP'), ('people', 'S-NP'), ('dying', 'B-VP'), ('make', 'E-VP'), ('this', 'B-NP'), ('world', 'E-NP'), ('a', 'B-NP'), ('better', 'I-NP'), ('place', 'E-NP'), ('for', 'S-PP'), ('you', 'S-NP'), ('and', 'O'), ('for', 'S-PP'), ('me.', 'S-NP')], 'pos': [('There', 'EX'), ('are', 'VBP'), ('people', 'NNS'), ('dying', 'VBG'), ('make', 'VB'), ('this', 'DT'), ('world', 'NN'), ('a', 'DT'), ('better', 'JJR'), ('place', 'NN'), ('for', 'IN'), ('you', 'PRP'), ('and', 'CC'), ('for', 'IN'), ('me.', '.')], 'srl': [{'A1': 'people', 'V': 'dying'}, {'A1': 'people  this world', 'A2': 'a better place for you and for me.', 'V': 'make'}], 'syntax_tree': '(S1(S(NP(EX There))(VP(VBP are)(NP(NP(NNS people))(SBAR(S(VBG dying)(VP(VB make)(S(NP(DT this)(NN world))(NP(DT a)(JJR better)(NN place)))(PP(PP(IN for)(NP(PRP you)))(CC and)(PP(IN for)(NP(. me.)))))))))))', 'verbs': ['dying', 'make'], 'words': ['There', 'are', 'people', 'dying', 'make', 'this', 'world', 'a', 'better', 'place', 'for', 'you', 'and', 'for', 'me.'], 'ner': [('There', 'O'), ('are', 'O'), ('people', 'O'), ('dying', 'O'), ('make', 'O'), ('this', 'O'), ('world', 'O'), ('a', 'O'), ('better', 'O'), ('place', 'O'), ('for', 'O'), ('you', 'O'), ('and', 'O'), ('for', 'O'), ('me.', 'O')]}

\>\>\>fannotator.getAnnotations("Biplab is a good boy.")['pos']
[('Biplab', 'NNP'), ('is', 'VBZ'), ('a', 'DT'), ('good', 'JJ'), ('boy', 'NN'), ('.', '.')]
  
\>\>\>fannotator.getAnnotations("Biplab is a good boy.")['ner']
[('Biplab', 'S-PER'), ('is', 'O'), ('a', 'O'), ('good', 'O'), ('boy', 'O'), ('.', 'O')]
  </pre>

