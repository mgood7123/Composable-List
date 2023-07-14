# Composable-List
a C++ Composable List, featuring both immutable and mutable variants, similar to Java.Function.andThen

# example

```cpp
int main() {

    auto ex = [](auto * n) { if (n != nullptr) n->operator()(); };

    #define M(v) []() { std::cout << #v << std::endl; }

    {
        // a
        auto a = new Composable_List<void> (M( a ));
        a->visitFromStart(ex);
        M()();
        // a, b
        auto b = a->after(M( b ));
        b->visitFromStart(ex);
        M()();
        // b, a, b
        auto b1 = a->before(M( b ));
        b1->visitFromStart(ex);
        M()();
        // b, a, c, b
        auto c = a->after(M( c ));
        c->visitFromStart(ex);
        M()();
        // b, d, a, c, b
        auto d = a->before(M( d ));
        d->visitFromStart(ex);
        M()();
        // b, e, d, a, c, b
        auto e = d->before(M( e ));
        e->visitFromStart(ex);
        M()();
        // b, e, d, f, a, c, b
        auto f = d->after(M( f ));
        f->visitFromStart(ex);
        M()();
        // b, e, d, f, g, a, c, b
        auto g = f->after(M( g ));
        g->visitFromStart(ex);
        M()();
        // b, e, d, f, g, a, c, b
        b->visitFromStart(ex);
    } {
        // a
        auto a = new Composable_List_COW<void> (M( a ));
        a->visitFromStart(ex);
        M()();
        // a, b
        auto b = a->after(M( b ));
        b->visitFromStart(ex);
        M()();
        // b, a, b
        auto b1 = b->before(M( b ));
        b1->visitFromStart(ex);
        M()();
        // b, a, c, b
        auto c = b1->after(a, M( c ));
        c->visitFromStart(ex);
        M()();
        // b, d, a, c, b
        auto d = c->before(a, M( d ));
        d->visitFromStart(ex);
        M()();
        // b, e, d, a, c, b
        auto e = d->before(d, M( e ));
        e->visitFromStart(ex);
        M()();
        // b, e, d, f, a, c, b
        auto f = e->after(d, M( f ));
        f->visitFromStart(ex);
        M()();
        // b, e, d, f, g, a, c, b
        auto g = f->after(f, M( g ));
        g->visitFromStart(ex);
        M()();
        // a, b
        b->visitFromStart(ex);
    }

    return 0;
}
```
