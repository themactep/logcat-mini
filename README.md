# logcat-mini

Simple, reduced feature logcat replacement written in c to provide appropriate features for embedded devices without the bloat of c++ etc.

After strip and upx compression we come in around half the size of the nexrt smallest logcat cli tool I've found, by
enabling only the default (aka run without arguments) functionality and using C without C++ being mixed in.
