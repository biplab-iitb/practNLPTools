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
