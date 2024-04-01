# supermarket
## Task specification

The task is to develop classes to implement basic store/sell operations in a supermarket.

Assume a store in a supermarket. A lot of various articles are in the store, each article is unambiguously identified by its name (a string of a reasonable length, mostly up to 30 characters). Moreover, we keep the information on expiry date and the number of items we have in the store.

Class (CSupermarket) must provide the following operations:

- default constructor
- initializes an empty instance of the store,
- store ( name, expireDate, cnt )
  - the method stores an article into the store. The article is identified by its name (name), expiry date (expireDate) and the number of items stored (cnt). Do not assume anything about the expiry dates. The article being stored may have expiry date earlier than an article that has already been stored (even if the names are the same).
- sell ( list )
  - the method sells some articles. The parameter is a shopping list of type list<pair<string,int>>. The list consists of records, each record identifies some article and the requested quantity. The method gets somewhat complicated. Our customers do not have neat handwriting, thus there is sometimes problem with the OCR software that scans the shopping lists. Some names in the shopping lists may contain typos. Our implementation must be prepared, thus the following ideas are used to identify the articles:
    - first, we search the article with an exact match (case sensitive search),
    - if there is not an exact match, we search for an article where the name differs in exactly one character (one character in the name is misspelled), again the search is case sensitive. If this unambiguously identifies an article, we use this article instead,
    - otherwise the search fails - no article of that name was found (this happens if no article name differs in exactly one character, or there are two or more articles where the names differ in one character).

  - If the article is found, the method expends the articles starting from the oldest (i.e., first sell the articles with the closest expiry date). If there are enough items to expend, the method expends the items and removes the corresponding record from the shopping list. If there is not enough items in the store, the methods expends the available items and decreases the count in the shopping list accordingly.

  - The method is a kind of a "transaction": it first searches the store for the requested articles (is it present, exact match, match with one different character). Subsequently, the method expends the items and updates the store and the shopping list.

- expired ( date )
  - the method lists the articles that expire before the date in parameter. The result is a list of articles (and the number of expiring items in the store) where the expiry date is strictly less than the date in the parameter. The resulting list is ordered according to the number of expiring items in a decreasing order. If there are two or more articles with the same number of expiring items, then there is not specified any relative ordering of the articles in such a group; any order will be accepted.
Submit a source of the classes CSupermarket and CDate. Use the attached file as a basis of your implementation. Moreover, the attached file contains some basic tests. You are free to add further classes to simplify your implementation.
