These are the embeddings we obtained with our language model approach.
They were used to initialize all our neural networks (for all tasks).
Each line in embeddings.txt is a 50-dimensional vector, which represent
the word found at the corresponding line in hash/words.lst.

See the JMLR paper "Natural Language Processing (Almost) from Scratch" and
the AISTATS paper "Deep Learning for Efficient Discriminative Parsing" for
more details.

These embeddings differ from Joseph Turian's embeddings (even though it is
unfortunate they have been called "Collobert & Weston embeddings" in
several papers).  Our embeddings have been trained for about 2 months, over
Wikipedia.
